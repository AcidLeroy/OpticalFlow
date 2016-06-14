function features = extract_feature_vectors(features_to_extract, data_dir) 
% extact_feature_vectors - This function extracts features from videos for
% later classification. 
% parameters: 
%    features_to_extract - either 'typing' or 'writing'
%    data_dir - This is the directory where the videos are found. 

switch features_to_extract 
   case 'typing'
      % Define the videos:
      Dir = strcat(data_dir ,'/typing/Cropped/');
      Videos = {'seg1.mov','seg4.mov', 'seg8.mov', 'seg9.mov', 'seg19.mov'};
      VideoList = strcat(Dir, Videos); 

      NoActionDir = strcat(data_dir, '/notyping/Cropped/');
      NoActionVideos= {'seg1.mov','seg9.mov', 'seg11.mov',  'seg12.mov'}; 
      NoActionVideoList = strcat(NoActionDir, NoActionVideos); 
      feature_name = 'Typing'; 


   case 'writing'
      % Define the videos:
      Dir = strcat(data_dir, '/writing/Cropped/');
      Videos = {'seg2.mov', 'seg6.mov','seg7.mov','seg18.mov','seg20.mov'};
      VideoList = strcat(Dir, Videos); 
      
      NoActionDir = strcat(data_dir, '/nowriting/Cropped/');
      NoActionVideos = {'seg1.mov',  ...
          'seg14.mov', 'seg16.mov',  'seg17.mov'}; 
      NoActionVideoList = strcat(NoActionDir, NoActionVideos); 
      feature_name = 'Writing';  
  
   otherwise
      error('Unknown feature "%s" to extract! Aborting...', feature_to_extract); 

end

ClassVectors = [];
ClassVectors = extract_features(VideoList, ClassVectors, sprintf('%s', feature_name), 0);
ClassVectors = extract_features(NoActionVideoList, ClassVectors, sprintf('No%s', feature_name), 1);

filename = sprintf('VideoHistos_%s', feature_name); 
save(filename, 'ClassVectors', '-ascii', '-tabs');    

function ClassVectors = extract_features(VideoList, ClassVectors, filename, classification)
   method = 'HS';
   for i = 1:length(VideoList)
      Video_str  = char(VideoList{i});
      disp(sprintf('Processing %s', Video_str));

      % Process the video
      VisFlow   = StdMotionEst(Video_str, method);
      [CenX_CDF, CenY_CDF, Orient_CDF, Histo_CDF, Motion_mag_CDF, Motion_orient_CDF] ...
        = VisFlow.ProcessAndDisp();

      % Store the results in a .mat file.
      loc  = strfind (Video_str, '.mp4');
      name = sprintf('%s%s.mat', filename, Video_str(1:loc-1)); 

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
                      classification];
      ClassVectors = [ClassVectors; NewVector];
   end
end

end


