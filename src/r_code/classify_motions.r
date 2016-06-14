# Clean up all variables and functions:
rm(list=ls())

library(class)
library(e1071)

# Function definitions:

# Assumptions:
#   VideoHists contains row-vectors to be classified.
#   Last column of VideoHists is the class (-1 or +1).

PlotHistsClasses <- function(VideoHists){
  # Visualize the different classes
  d<-dim(VideoHists);
  NumOfVars <- d[2];
  
  # Select class=1 or -1
  VideoHists1 <- VideoHists[which(VideoHists[, NumOfVars] == +1), seq(1:(NumOfVars-1))];
  VideoHists2 <- VideoHists[which(VideoHists[, NumOfVars] == +2), seq(1:(NumOfVars-1))];
  
  library(ggplot2);
  par(mfrow=c(6,2));
  par(mar=c(1,1,1,1))
  plot(melt(VideoHists1[, seq(1,25)]),
       main="1: CenX CDF", 
       xlab="x", 
       ylab="CDF");
  plot(melt(VideoHists2[, seq(1,25)]),
       main="2: CenX CDF", 
       xlab="x", 
       ylab="CDF");
  
  plot(melt(VideoHists1[, seq(25+1, 25+25)]),
       main="1: CenY CDF", 
       xlab="y", 
       ylab="CDF");
  plot(melt(VideoHists2[, seq(25+1, 25+25)]),
       main="2: CenY CDF", 
       xlab="y", 
       ylab="CDF");
  
  plot(melt(VideoHists1[, seq(50+1,50+25)]),
       main="1: Orient CDF", 
       xlab="Angle", 
       ylab="CDF");
  plot(melt(VideoHists2[, seq(50+1,50+25)]),
       main="2: Orient CDF", 
       xlab="Angle", 
       ylab="CDF");  
  
  plot(melt(VideoHists1[, seq(75+1,75+25)]),
       main="1: Orient CDF", 
       xlab="Angle", 
       ylab="CDF");
  plot(melt(VideoHists2[, seq(75+1,75+25)]),
       main="2: Orient CDF", 
       xlab="Angle", 
       ylab="CDF"); 
  
  plot(melt(VideoHists1[, seq(100+1,100+25)]),
       main="1: Orient CDF", 
       xlab="Angle", 
       ylab="CDF");
  plot(melt(VideoHists2[, seq(100+1,100+25)]),
       main="2: Orient CDF", 
       xlab="Angle", 
       ylab="CDF"); 
  
  plot(melt(VideoHists1[, seq(125+1,125+25)]),
       main="1: Orient CDF", 
       xlab="Angle", 
       ylab="CDF");
  plot(melt(VideoHists2[, seq(125+1,125+25)]),
       main="2: Orient CDF", 
       xlab="Angle", 
       ylab="CDF"); 
}

FeatureSelection <- function(VideoHists){
  # Selects features that produce different variables:
  d<-dim(VideoHists);
  NumOfVars <- d[2];
  
  # Select class=1 or -1
  VideoHists1 <- VideoHists[which(VideoHists[, NumOfVars] == +1), seq(1:(NumOfVars-1))];
  VideoHists2 <- VideoHists[which(VideoHists[, NumOfVars] == +2), seq(1:(NumOfVars-1))];
  
  # Go through and construct only the variables that are different:
  ValidVars <- 0;
  TestedVideoHists = VideoHists;
  for (i in 1:(NumOfVars-1)) {
    # Compare every variable:
    x = VideoHists1[,i];
    y = VideoHists2[,i];
    
    # Nothing to do if same mean:
    if (abs(mean(x)-mean(y)) > 1.0e-6) {
      # Compare the two populations:
      w <- wilcox.test(x, y, alternative = "two.sided");
      if (w$p.value < 0.05) { # 0.02
        # They are different.
        # Use this variable.
        ValidVars <- ValidVars + 1;
        TestedVideoHists[, ValidVars] <- VideoHists[,i]; 
      } 
    }
  }
    
  # Attach the classification results at the end:
  ValidVars <- ValidVars + 1;
  TestedVideoHists[, ValidVars] <- VideoHists[, NumOfVars];
  
  # Eliminate unused variables:
  TestedVideoHists <- TestedVideoHists[, 1:ValidVars];
  
  # Return the variable:
  TestedVideoHists
}

PlotFeatureClasses <- function(VideoHists){
  # Visualize the different classes
  d<-dim(VideoHists);
  NumOfVars <- d[2];
  
  # Select class=1 or -1
  VideoHists1 <- VideoHists[which(VideoHists[, NumOfVars] == 1), seq(1:(NumOfVars-1))];
  VideoHists2 <- VideoHists[which(VideoHists[, NumOfVars] == 2), seq(1:(NumOfVars-1))];
  
  library(ggplot2);
  
  par(mfrow=c(1,2));
  plot(melt(VideoHists1),
       main="1: Selected Features", 
       xlab="x", 
       ylab="Values");
  plot(melt(VideoHists2),
       main="2: Selected Features", 
       xlab="x", 
       ylab="Values");
}

ClassifyFeatures <- function(VideoHists){
  dims <- dim(VideoHists);
  NoOfSamples <- dims[1];
  NoOfVars    <- dims[2];
  
  # Build a factor of the correct classification:
  All_cl <- VideoHists[, NoOfVars];
  
  # Store 1 for wrong classification and 0 for correct.
  knnResult <- rep(1, times=NoOfSamples);
  svmResult <- rep(1, times=NoOfSamples);
  
  # Create a leave one out classification approach
  for(i in 1:NoOfSamples)
  {
    # Set up training and testing data:
    trainData = VideoHists[-i, 1:NoOfVars-1]; # Remove i.
    testData  = VideoHists[i,  1:NoOfVars-1]; # One left out.
    
    # Prepare the labels for the training set:
    #  Optimal: k=1
    knnResult[i] <- knn (trainData, testData, All_cl[-i], k=3); # 3
    
    #**** With tuning ****#
    #print(All_cl[-i])
    #tune.out=tune(svm,trainData,All_cl[-i],kernel="linear",ranges=list(cost=c(0.001, 0.01, 0.1, 1,5,10,100)))
    #tune.out=tune(svm,All_cl[-i] ~ .,data=trainData,kernel="linear",ranges=list(cost=c(0.001, 0.01, 0.1, 1,5,10,100)))
    #svmResult[i] <- predict(tune.out$best.model, testData);
    #print(All_cl);
    #print(round(svmResult));
    
    #***** Without tuning *****#
    model <- svm(All_cl[-i] ~ ., data=trainData);
    svmResult[i] <- predict(model, testData);
    
    
    
  }
  
  print(All_cl);
  print(knnResult)
  
  print(All_cl);
  print(round(svmResult));
        
  cat("Confusion Matrix: knn");
  print(table(All_cl, knnResult));
  cat("Confusion Matrix: SVM");
  print(table(All_cl, round(svmResult)));
}

# Read the table
#VideoHists = read.table("E:/VJ/AOLME/PreProposal_AOLMEFall2015/ActivityClassifier/data/FeatureVectorsOutput/VideoHistos_WriVsNoWri.txt", header=F);

#VideoHists = read.table("C:/Users/user/Dropbox/ICIP2016/VideoHistos_TrackVsZoom.txt", header=F);
#VideoHists = read.table("C:/Users/user/Dropbox/ICIP2016/VideoHistos_ZoomVsStat.txt", header=F);
VideoHists = read.table("../FeatureVectors/VideoHistos_WriVsNoWri.txt", header=F);
# Convert classification results to 1-2 instead of 0-1
VideoHists[, 151] <- VideoHists[, 151] + 1;

# We want to visualize the CDF plots.
library(reshape2);

# Plot the classes:
PlotHistsClasses(VideoHists)

# Select the features:
ExtractedFeatures <- FeatureSelection(VideoHists)
  
# Plot extracted features:
#PlotFeatureClasses(ExtractedFeatures)
  
# Classification
# Loads the classification package in R.

ClassifyFeatures(ExtractedFeatures)


