# Clean up all variables and functions:
rm(list=ls())
library(class)
library(e1071)

# Function definitions:

# Plots a particular column in the vector with a given label name
# dataframe - This is the data frame that contains a list for each respective CDF
# column_name - This is the column for which to plot the box and whiskers for all the values in this class. 
# x_label - This is the label for the type of data being plotted in the CDF
PlotClass <- function(dataframe, column_name, x_label){
  dataset = dataframe[[column_name]]
  mat = do.call(rbind, dataset)
  boxplot(mat, use.cols = TRUE, main=column_name, xlab=x_label, ylab='CDF')
}

# Returns all rows that match a particular classification type. I.e. if you want to get 
# all values in the data frame that were classified as a 1, you would simply call
# GetClassification(df 1). 
GetClassification <- function(dataframe, classification){
  class_tf = unlist(lapply(dataframe$Classification, function(x) x == classification))
  return (sapply(dataframe, function(x) x[class_tf], simplify=FALSE))
}

# Plot CDFs found in the dataframe. 
PlotHistsClasses <- function(VideoHists){
  VideoHists1 =  GetClassification(VideoHists, 1)
  VideoHists2 =  GetClassification(VideoHists, 2)
  
  library(ggplot2);
  par(mfrow=c(6,2));
  par(mar=c(1,1,1,1))
  
  no_plot = "Classification"
  x_labels = c('x', 'y', 'angle', 'angle', 'motion magnitude', 'angle')
  idx = 1; 
  for( i in names(VideoHists[!names(VideoHists) %in% no_plot])) {
    PlotClass(VideoHists1, i, x_labels[idx])
    PlotClass(VideoHists2, i, x_labels[idx])
    idx = idx + 1
  }

}


# Get all columns except for the ones specified in excluded_columns. Returns the modified 
# data frame
GetAllExcept <- function(df, excluded_columns) {
  return (df[names(df[!names(df) %in% excluded_columns])])
}

# Combine features of that dataframe into a matrix so that the features
# can easily processed by svm and knn.
CombineFeatures <- function(df, features_to_combine) {
   result = do.call(cbind, df[features_to_combine])
   return (apply(result, 1, unlist))
}

FeatureSelection <- function(VideoHists){
  # Selects features that produce different variables:
  
  VideoHists1 =  GetClassification(VideoHists, 1)
  VideoHists2 =  GetClassification(VideoHists, 2)
  
  # Go through and construct only the variables that are different:
  ValidVars = c()

  # Loop through all variables except classifcation
  for( i in names(VideoHists[!names(VideoHists) %in% "Classification"])) {
    # Compare every variable:
    x = unlist(VideoHists1[[i]]);
    y = unlist(VideoHists2[[i]]);
    
    # Nothing to do if same mean:
    if (abs(mean(x)-mean(y)) > 1.0e-6) {
      # Compare the two populations:
      w <- wilcox.test(x, y, alternative = "two.sided");
      if (w$p.value < 0.05) { # 0.02
        # They are different.
        # Use this variable.
        ValidVars = c(ValidVars, i)
      } 
    }
  }
  
  # Attach classification
  ValidVars = c(ValidVars, "Classification")
  return (VideoHists[ValidVars])
 
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
  NoOfSamples <- length(VideoHists$Classification)
  
  # Build a factor of the correct classification:
  All_cl <- unlist(VideoHists$Classification);
  
  # Store 1 for wrong classification and 0 for correct.
  knnResult <- rep(1, times=NoOfSamples);
  svmResult <- rep(1, times=NoOfSamples);
  
  # Remove classification
  features = GetAllExcept(VideoHists, "Classification")
  
  # Create a leave one out classification approach
  for(i in 1:NoOfSamples)
  {
    # Set up training and testing data:
    trainData = lapply(features, function(x) x[-i]) # Remove i.
    testData  = lapply(features, function(x) x[i]) # One left out.
    
    #Combine data
    trainData = t(CombineFeatures(trainData, names(trainData)))
    testData = t(CombineFeatures(testData, names(testData)))
    
    # Prepare the labels for the training set:
    #  Optimal: k=1
    knnResult[i] <- knn (trainData, testData, All_cl[-i], k=3); # 3
    
    #**** With tuning ****#
    tune.out=tune(svm, trainData, All_cl[-i], , kernel="linear", ranges=list(cost=c(0.0001, 0.001, 0.01, 0.1, 1, 5, 10, 100, 10000)) , scale=FALSE)
    svmResult[i] <- predict(tune.out$best.model, testData);
    
    
    
    #***** Without tuning *****#
#    model <- svm(All_cl[-i] ~ ., data=trainData, scale=FALSE);
#    svmResult[i] <- predict(model, testData);
    cat("SVM result =  ", round(svmResult), "\n");
    cat("KNN result =  ", knnResult, "\n")
    
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
#VideoHists = read.table("/Users/cody/Repos/OpticalFlow/src/matlab/VideoHistos_Typing_cells.csv", header=F);
source("load_video_features.r")

list.of.packages <- c("ggplot2", "reshape2", "e1071", "class")
new.packages <- list.of.packages[!(list.of.packages %in% installed.packages()[,"Package"])]
if(length(new.packages)) install.packages(new.packages)


#VideoHists = LoadVideoFeatures("/Users/cody/Repos/OpticalFlow/src/matlab/VideoHistos_Typing_cells.csv")
VideoHists = LoadVideoFeatures("/Users/cody/Repos/OpticalFlow/build/bin/test.txt")

# We want to visualize the CDF plots.
library(reshape2);

# Plot the classes:
PlotHistsClasses(VideoHists)

# Select the features:
ExtractedFeatures <- FeatureSelection(VideoHists)
#ExtractedFeatures <- VideoHists[c("Motion_mag_CDF", "Classification")]
  
# Plot extracted features:
#PlotFeatureClasses(ExtractedFeatures)
  
# Classification
# Loads the classification package in R.

ClassifyFeatures(ExtractedFeatures)


