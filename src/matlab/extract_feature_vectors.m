function features = extract_feature_vectors(features_to_extract, data_dir) 
% extact_feature_vectors - This function extracts features from videos for
% later classification. 
% parameters: 
%    features_to_extract - either 'typing' or 'writing'
%    data_dir - This is the directory where the feature videos are found. 
%
% Example: 
%     extract_feature_vectors('typing', '/Users/cody/Dropbox/CodyShared/PreProposal_AOLMEFall2015/ActivityClassifier/data')

switch features_to_extract 
   case 'typing'
      % Define the videos:
      Dir = strcat(data_dir ,'/typing/Cropped/*.mp4');

      VideoList = ls(Dir); 
      VideoList = strsplit(VideoList, '\n')
      VideoList = VideoList(1:end-1);

      NoActionDir = strcat(data_dir, '/notyping/Cropped/*.mp4');
      NoActionVideoList = ls(NoActionDir); 
      NoActionVideoList = strsplit(NoActionVideoList, '\n');
      NoActionVideoList = NoActionVideoList(1:end-1);
       
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
      error('Unknown feature "%s" to extract! Please choose "typing" or "writing". Aborting...', features_to_extract); 

end

ClassVectors = [];
CellVectors = {'CenX_CDF', 'CenY_CDF', 'Orient_CDF', 'Histo_CDF', 'Motion_mag_CDF', 'Motion_orient_CDF', 'Classification'};
[ClassVectors, CellVectors] = extract_features(VideoList, ClassVectors, CellVectors, sprintf('%s', feature_name), 1);
[ClassVectors, CellVectors] = extract_features(NoActionVideoList, ClassVectors, CellVectors, sprintf('No%s', feature_name), 2);

filename = sprintf('VideoHistos_%s_vectors.csv', feature_name); 
save(filename, 'ClassVectors', '-ascii', '-tabs'); 
filename = sprintf('VideoHistos_%s_cells.csv', feature_name); 
cell2csv(filename, CellVectors, ',')
features = CellVectors;

function [ClassVectors, CellVectors] = extract_features(VideoList, ClassVectors, CellVectors, filename, classification)
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

%       eval(sprintf('save %s %s %s %s %s %s %s', name, ...
%                    'CenX_CDF', 'CenY_CDF', 'Orient_CDF', 'Histo_CDF', ...
%                    'Motion_mag_CDF', 'Motion_orient_CDF'));

      % Update ClassVectors
      NewVector    = [transpose(CenX_CDF(:)), ...
                      transpose(CenY_CDF(:)), ...
                      transpose(Orient_CDF(:)), ...
                      transpose(Histo_CDF(:)), ...
                      transpose(Motion_mag_CDF(:)), ...
                      transpose(Motion_orient_CDF(:)), ...
                      classification];
      ClassVectors = [ClassVectors; NewVector];
      CellVectors = vertcat(CellVectors, {transpose(CenX_CDF(:)), ...
                      transpose(CenY_CDF(:)), ...
                      transpose(Orient_CDF(:)), ...
                      transpose(Histo_CDF(:)), ...
                      transpose(Motion_mag_CDF(:)), ...
                      transpose(Motion_orient_CDF(:)), ...
                      classification});
   end
end

end


