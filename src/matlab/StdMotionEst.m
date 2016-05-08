%%  StdMotionVideo: 
% Computes motion over the entire video.
% See example on how to run:
%    >>StdMotionEst.demo();
classdef StdMotionEst < handle % Inherit all of the properties
    properties (Access=public) % everyone can access
        VideoName        % Name of the file to process.
        MotionEstMethod  % Motion estimation method ('HS', 'LK', 'FB').
        
        CentroidsX       % Centroid X for each component.
        CentroidsY       % Centroid Y for each component.
        Orientations     % Orientations for each component.
        
        Histo             % Background historgram.
        MotionMagHisto    % Motion vector magnitude histogram.
        MotionOrientHisto % Motion vector orientation histogram.
        
        LastElement      % A pointer to the last component.
    end
    
    properties (Access=private) % Available in this file only. 
        vidReader % VideoReader Matlab class object.
        opticFlow % Optical flow Matlab class object.
    end
    
    %% Public methods: Constructor, Process, and Display for users.
    methods (Access=public)
        function imgObj = StdMotionEst(VideoName, MotionEstMethod)
            % Set up the motion estimation method to run on given video.
            %  VideoName: is the name of the video to process.
            %  MotionEstMethod: 
            %   'HS' for Horn and Schunk.
            %   'LK' for Lucas and Kanade.
            %   'FB' for Farnenback.
         
            % Public properties:
            imgObj.VideoName       = VideoName;
            imgObj.MotionEstMethod = MotionEstMethod;
            
            % Setup internal private reader object:            
            imgObj.vidReader = VideoReader(VideoName);

            % Setup internal motion estimation object:
            imgObj.SetMotionEstMethod(MotionEstMethod);
            
            % Initialize component estimation
            imgObj.LastElement  = 0;
            imgObj.CentroidsX   = zeros(10000,1);
            imgObj.CentroidsY   = zeros(10000,1);
            imgObj.Orientations = zeros(10000,1);
            
            % Background histogram initialization:
            imgObj.Histo        = zeros(25,1);
            
            % Motion vector histograms:
            imgObj.MotionMagHisto    = zeros(25,1); 
            imgObj.MotionOrientHisto = zeros(25,1);
        end
        
        function [CenX_CDF, CenY_CDF, Orient_CDF, Histo_CDF, Motion_mag_CDF, ...
                  Motion_orient_CDF] = ProcessAndDisp(imgObj, fname)
            % Process and display motion estimation for the video.
            frameNum = 1;
            while hasFrame(imgObj.vidReader)
              frameRGB = readFrame(imgObj.vidReader);
              frameGray = rgb2gray(frameRGB);
              
              flow = estimateFlow(imgObj.opticFlow,frameGray);

              % Threshold the magnitude:
              minValue    = min(flow.Magnitude(:));
              maxValue    = max(flow.Magnitude(:));
              medianValue = median(flow.Magnitude(:));
              
              Threshold = 0.25*maxValue;
              Comp = (flow.Magnitude > Threshold);
              % commented out by vj,  imgObj.PlotFlowComp(Comp, frameNum, frameRGB, flow, Threshold);
              
              frameNum = frameNum+1;
              
              imgObj.UpdateStats(Comp, frameGray, flow.Magnitude, flow.Orientation);
            end
            
            % Plot them all
            imgObj.PlotAllHistos(imgObj.VideoName);
            
            % Compute the CDFs:
            Xmax = size(Comp, 2);
            Ymax = size(Comp, 1);
            [CenX_CDF, CenY_CDF, Orient_CDF, Histo_CDF, Motion_mag_CDF, Motion_orient_CDF] = ...
               imgObj.ComputeCDFs (Xmax, Ymax);
        end
        
        function [CenX_CDF, CenY_CDF, Orient_CDF, Histo_CDF, Motion_mag_CDF, Motion_orient_CDF] = ...
                    ComputeCDFs (imgObj, Xmax, Ymax)
                
            NumOfBins = 25;
            
            % Confirmed Xmax, Ymax
            h_edges  = linspace(1, Xmax, NumOfBins);
            N        = histc(imgObj.CentroidsX(:), h_edges);
            CenX_CDF = cumsum(N)./sum(N(:));
            
            h_edges  = linspace(1, Ymax, NumOfBins);
            N        = histc(imgObj.CentroidsY(:), h_edges);
            CenY_CDF = cumsum(N)./sum(N(:));
            
            % Confirmed range.
            h_edges    = linspace(-90, +90, NumOfBins);
            N          = histc(imgObj.Orientations(:), h_edges);
            Orient_CDF = cumsum(N)./sum(N(:));
            
            % We have the histogram. Normalize:
            Histo_CDF  = cumsum(imgObj.Histo)./sum(imgObj.Histo(:));
            Motion_mag_CDF    = cumsum(imgObj.MotionMagHisto)./sum(imgObj.MotionMagHisto(:));
            Motion_orient_CDF = cumsum(imgObj.MotionOrientHisto)./sum(imgObj.MotionOrientHisto(:));
        end
        
        function PlotFlowComp(imgObj, Comp, frameNum, frameRGB, flow, Threshold)
            % 1. Determine the magnitude of the motion to threshold
            % 2. Extract the componennts.
            % 3. Get 'Orientation' using regionprops
            % 4. Generate CDF/histogram of orientations for classification.
            % 5. Generate CDF/histogram of centroid x.
            % 6. Generate CDF/histogram of centroid y.
            figure(1);
            subplot(4,1,1);
            imshow(frameRGB)
            hold on
            plot(flow,'DecimationFactor',[5 5],'ScaleFactor', 50);
            hold off
            title(sprintf('Method=%s frame=%.0f', imgObj.MotionEstMethod, frameNum));
            
            subplot(4,1,2);
            imagesc(flow.Magnitude), axis image, colormap(gray);
            title('Magnitude of the motion');
            
            subplot(4,1,3);
            imagesc(Comp), axis image, colormap(gray);
            title(sprintf('Extracted component t=%f', Threshold));  
            
            subplot(4,1,4);
            [N, X]  = hist(flow.Magnitude(:), 100);
            N(1:10) = 1;
            plot(X, N);
            title('Histogram of the motion magnitude');
            
            drawnow
        end
        
        function UpdateStats(imgObj, Comp, frameGray, mags, orients)
           % Updates histogram stats based on the frame.
           
           CC = bwconncomp(Comp); 
           s  = regionprops(CC,'Orientation', 'Centroid');
           L  = imgObj.LastElement;
           for i=1:length(s)
               imgObj.CentroidsX(L+i)   = s(i).Centroid(1);
               imgObj.CentroidsY(L+i)   = s(i).Centroid(2);
               imgObj.Orientations(L+i) = s(i).Orientation;
           end
           imgObj.LastElement = imgObj.LastElement + length(s);
           
           % Dilate and get the histogram of the extra component.
           SEsize = 5;
           imgObj.GetHistoAround(Comp, SEsize, frameGray);
           
           % Get the motion vectors inside the components
           inside_mags    = mags(logical(Comp));
           inside_orients = orients(logical(Comp));
           
           % Update the histograms:
           NumOfBins = 25;
           h_edges   = linspace(0, 0.1, NumOfBins);   
           N         = histc(inside_mags(:), h_edges);
           imgObj.MotionMagHisto = imgObj.MotionMagHisto + N;
           
           h_edges   = linspace(-pi, +pi, NumOfBins);
           N         = histc(inside_orients(:), h_edges);
           imgObj.MotionOrientHisto = imgObj.MotionOrientHisto + N;
        end
        
        function GetHistoAround(imgObj, Comp, SEsize, frameGray)
          % Get the background of the motion:  
          se = strel('disk', SEsize);
          d = imdilate (Comp, se);
          dimg = (d - Comp);
          
          % Update histogram from the background:
          backPixels = frameGray(logical(dimg));
          
          NumOfBins    = 25;
          h_edges      = linspace(0, 255, NumOfBins);
          N            = histc(backPixels(:), h_edges);
          imgObj.Histo = imgObj.Histo + N;
        end
        
        function PlotAllHistos(imgObj, str)
            figure
            title(str);
            
            NumOfBins = 25;
            L = imgObj.LastElement;
            
            disp(sprintf('L = %d', L));
            
            subplot(6,2,1);
            plotHisto(imgObj.CentroidsX(1:L), NumOfBins, 'Centroid X histo');
            subplot(6,2,2);
            plotCDF(imgObj.CentroidsX(1:L), NumOfBins, 'Centroid X CDF');
            
            subplot(6,2,3);
            plotHisto(imgObj.CentroidsY(1:L), NumOfBins, 'Centroid Y histo');
            subplot(6,2,4);
            plotCDF(imgObj.CentroidsY(1:L), NumOfBins, 'Centroid Y CDF');
            
            subplot(6,2,5);
            plotHisto(imgObj.Orientations(1:L), NumOfBins, 'Orientations histo');
            subplot(6,2,6);
            plotCDF(imgObj.Orientations(1:L), NumOfBins, 'Orientations CDF');
            
            subplot(6,2,7);
            plot(imgObj.Histo);
            title('Surround values histogram');
            subplot(6,2,8);
            plot(cumsum(imgObj.Histo)./sum(imgObj.Histo(:)));
            title('Surround values CDF');
            
            subplot(6,2,9);
            plot(imgObj.MotionMagHisto);
            title('Motion magnitude histo');
            subplot(6,2,10);
            plot(cumsum(imgObj.MotionMagHisto)./sum(imgObj.MotionMagHisto(:)));
            title('Motion magnitude CDF');
            
            subplot(6,2,11);
            plot(imgObj.MotionOrientHisto);
            title('Motion orientation histo');
            subplot(6,2,12);
            plot(cumsum(imgObj.MotionOrientHisto)./sum(imgObj.MotionOrientHisto(:)));
            title('Motion orientation CDF');
        end
        
        function PlotFlow(imgObj, frameNum, frameRGB, flow)
            % Plots input video, abs(Vx), abs(Vy), orientation,
            % and magnitude of the motion.
              subplot(5,1,1);
              imshow(frameRGB)
              hold on
              plot(flow,'DecimationFactor',[5 5],'ScaleFactor', 50);
              hold off
              title(sprintf('Method=%s frame=%.0f', imgObj.MotionEstMethod, frameNum));
              
              subplot(5,1,2);
              imagesc(abs(flow.Vx)), axis image, colormap(gray);
              title('abs(Vx) component');
              
              subplot(5,1,3);
              imagesc(abs(flow.Vy)), axis image, colormap(gray);
              title('abs(Vy) component');
              
              subplot(5,1,4);
              imagesc(flow.Magnitude), axis image, colormap(gray);
              title('Magnitude of the motion');
              
              subplot(5,1,5);
              imagesc(flow.Orientation), axis image, colormap(gray);
              title('Orientation of the motion');
        end
    end
    
    %% Private methods: Not visible to outside objects.
    methods (Access=private)
        function SetMotionEstMethod(imgObj, MotionEstMethod)
            %  MotionEstMethod: 
            %   'HS' for Horn and Schunk.
            %   'LK' for Lucas and Kanade.
            %   'FB' for Farnenback.
            switch (MotionEstMethod)
             case 'HS'
                imgObj.opticFlow = opticalFlowHS(...
                                       'Smoothness',   10, ...
                                       'MaxIteration', 100);   
             case 'LK'
                imgObj.opticFlow = opticalFlowLKDoG(...
                                       'NoiseThreshold', 0.0005);  
             case 'FB'
                imgObj.opticFlow = opticalFlowFarneback;
             otherwise
              error(sprintf('%s not recognized', VideoName));         
             end
        end
    end
    
    %% Static methods: demo() for users to see the class in action.
    %  For these static methods, no need to construct an object.
    %     >>StdMotionEst.demo()  
    methods(Static, Access=public)
         function demo()
           % Demonstrates the use of the class on an internal object.
            figure
            disp('Method=HS');
            DemoFlow = StdMotionEst('visiontraffic.avi', 'HS');
            DemoFlow.ProcessAndDisp();
            
            figure
            disp('Method=LK');
            DemoFlow = StdMotionEst('visiontraffic.avi', 'LK');
            DemoFlow.ProcessAndDisp();
            
            figure
            disp('Method=FB');
            DemoFlow = StdMotionEst('visiontraffic.avi', 'FB');
            DemoFlow.ProcessAndDisp();
         end
     end
end

