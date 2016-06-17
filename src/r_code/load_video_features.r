LoadVideoFeatures <- function(csv_file){
  options( stringsAsFactors=F )
  df = read.csv(csv_file)
  data_to_numeric_list = function(col) lapply(col, function(x) as.numeric(strsplit(x, "\\s+")[[1]]))
  return(apply(df, 2, data_to_numeric_list))
}

