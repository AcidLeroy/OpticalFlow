function extract_features( classification, video_file, method )
%EXTRACT_FEATURES Extract features from a video given a specific 
% classification 
%path_to_video = coder.newtype('constant', video_file);
path_to_video = video_file;
VisFlow   = StdMotionEst(path_to_video, method);
[CenX_CDF, CenY_CDF, Orient_CDF, Histo_CDF, Motion_mag_CDF, Motion_orient_CDF] ...
        = VisFlow.ProcessAndDisp();
     
     
FeatureVector = sprintf('%s,%s,%s,%s,%s,%s,%d\n', ...
   remove_brackets(mat2str(transpose(CenX_CDF(:)))), ...
   remove_brackets(mat2str(transpose(CenY_CDF(:)))), ...
   remove_brackets(mat2str(transpose(Orient_CDF(:)))), ...
   remove_brackets(mat2str(transpose(Histo_CDF(:)))), ...
   remove_brackets(mat2str(transpose(Motion_mag_CDF(:)))), ...
   remove_brackets(mat2str(transpose(Motion_orient_CDF(:)))), ...
   classification); 
   
fprintf(FeatureVector)
            
end

function result = remove_brackets(input)
   result = strrep(input, '[', '');
   result = strrep(result, ']', ''); 
end

