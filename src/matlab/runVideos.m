% Clear memory
%clear all;
%close all;

% Execute writitng or typing!
WRITING = true;
TYPING  = false;

if TYPING
% Define the videos:
Dir = '../data/typing/Cropped';
VideoList = {'seg1.mov','seg4.mov', 'seg8.mov', 'seg9.mov', 'seg19.mov'};

NoActionDir = '../data/notyping/Cropped';
NoActionVideoList = {'seg1.mov','seg9.mov', 'seg11.mov',  'seg12.mov'};    
end

if WRITING
% Define the videos:
Dir = '/Users/cody/Dropbox/CodyShared/PreProposal_AOLMEFall2015/ActivityClassifier/data/writing/Cropped';
VideoList = {'seg2.mov', 'seg6.mov','seg7.mov','seg18.mov','seg20.mov'};

NoActionDir = '/Users/cody/Dropbox/CodyShared/PreProposal_AOLMEFall2015/ActivityClassifier/data/nowriting/Cropped';
NoActionVideoList = {'seg1.mov',  ...
    'seg14.mov', 'seg16.mov',  'seg17.mov'};    
end


% Demonstrates the use of the class on an internal object.
method = 'HS';
ClassVectors = [];
for i = 1:length(VideoList)
   Video_str  = char(VideoList{i});
   disp(sprintf('Processing %s/%s', Dir, Video_str));

   % Process the video
   Full_name = sprintf('%s/%s', Dir, Video_str);
   VisFlow   = StdMotionEst(Full_name, method);
   [CenX_CDF, CenY_CDF, Orient_CDF, Histo_CDF, Motion_mag_CDF, Motion_orient_CDF] ...
     = VisFlow.ProcessAndDisp();

   % Store the results in a .mat file.
   loc  = strfind (Video_str, '.mp4');
   if TYPING
       name = sprintf('Typing%s.mat', Video_str(1:loc-1));
   end
   if WRITING
       name = sprintf('Writing%s.mat', Video_str(1:loc-1));
   end
   name = sprintf('Typing%s.mat', Video_str(1:loc-1));
   eval(sprintf('save %s %s %s %s %s %s %s', name, ...
                'CenX_CDF', 'CenY_CDF', 'Orient_CDF', 'Histo_CDF', ...
                'Motion_mag_CDF', 'Motion_orient_CDF'));

   % Update ClassVectors
   NewVector    = [transpose(CenX_CDF(:)), ...
                   transpose(CenY_CDF(:)), ...
                   transpose(Orient_CDF(:)), ...
                   transpose(Histo_CDF(:)), ...
                   transpose(Motion_mag_CDF(:)), ...
                   transpose(Motion_orient_CDF(:)), ...
                   0];
   ClassVectors = [ClassVectors; NewVector];
end

for i = 1:length(NoActionVideoList)
   Video_str  = char(NoActionVideoList{i});
   disp(sprintf('Processing NoTyping %s/%s', NoActionDir, Video_str));

   % Process the video
   Full_name = sprintf('%s/%s', NoActionDir, Video_str);
   VisFlow = StdMotionEst(Full_name, method);
   [CenX_CDF, CenY_CDF, Orient_CDF, Histo_CDF, Motion_mag_CDF, Motion_orient_CDF] ...
       = VisFlow.ProcessAndDisp();

   % Store the results in a .mat file.
   loc  = strfind (Video_str, '.mp4');
   if TYPING
       name = sprintf('NoTyping%s.mat', Video_str(1:loc-1));
   end
   if WRITING
       name = sprintf('NoWriting%s.mat', Video_str(1:loc-1));
   end
   eval(sprintf('save %s %s %s %s %s %s %s', name, ...
                'CenX_CDF', 'CenY_CDF', 'Orient_CDF', 'Histo_CDF', ...
                'Motion_mag_CDF', 'Motion_orient_CDF'));

   % Update ClassVectors
   NewVector    = [transpose(CenX_CDF(:)), ...
                   transpose(CenY_CDF(:)), ...
                   transpose(Orient_CDF(:)), ...
                   transpose(Histo_CDF(:)), ...
                   transpose(Motion_mag_CDF(:)), ...
                   transpose(Motion_orient_CDF(:)), ...
                   1];
   ClassVectors = [ClassVectors; NewVector];
end

if TYPING
% Save the results
save 'VideoHistos_TypVsNoTyp.txt' ClassVectors -ascii -tabs    
end

if WRITING
% Save the results
save 'VideoHistos_WriVsNoWri.txt' ClassVectors -ascii -tabs    
end

