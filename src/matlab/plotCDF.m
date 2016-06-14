function [] = plotCDF(ValuesArray, NumOfBins, TitleString)
%   plotCDF(ValuesArray, NumOfBins, TitleString)
%
% Input:
%   ValuesArray: Array of values.
%   NumOfBins:   Number of Bins for the histogram.
%   TitleString: String that will be used for the plot.
%
% Output:
%   Plots the cumulative distribution function (CDF).
%
% Example:
%   display('Compare histogram with CDF');
%   ValuesArray = randn(100, 1);
%   str = 'Histogram with too many bins: see many zeros!';
%   figure, plotHisto(ValuesArray, 100, str);
%   str = 'CDF for 100 bins is nice and smooth!';
%   figure, plotCDF(ValuesArray, 100, str);
%
%   str = 'Histogram with enough bins: see roughness';
%   figure, plotHisto(ValuesArray, 10,  str);
%   str = 'CDF for 10 bins is ok but not necessarilly better than 10 bins!';
%   figure, plotCDF(ValuesArray, 100, str);
%
%   display('plotCDF() is more accurate for more bins!');
%   display('CDF is only noisy at lower values.');
%   display('CDF is excellent for higher values.');
%
[N,X] = hist(ValuesArray(:), NumOfBins);
plot(X,cumsum(N)./sum(N(:)));
title(TitleString);