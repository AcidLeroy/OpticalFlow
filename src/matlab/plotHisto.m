function [] = plotHisto(ValuesArray, NumOfBins, TitleString)
%   plotHisto(ValuesArray, NumOfBins, TitleString)
% Input:
%   ValuesArray: Array of values.
%   NumOfBins:   Number of Bins for the histogram.
%   TitleString: String that will be used for the plot.
% Output:
%   Plots the hisrogram.
% Example:
%   ValuesArray = randn(100, 1);
%   str = 'Histogram with too many bins: see many zeros!';
%   figure, plotHisto(ValuesArray, 100, str);
%   str = 'Histogram with enough bins: see roughness';
%   figure, plotHisto(ValuesArray, 10,  str);
%   display('Consider plotCDF() also');

[N,X] = hist(ValuesArray(:), NumOfBins);
plot(X,N);
title(TitleString);
