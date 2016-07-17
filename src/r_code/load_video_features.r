LoadVideoFeatures <- function(csv_file){
  options( stringsAsFactors=F )
  df = read.csv(csv_file)
  df_no_filename = df[ ,!(colnames(df) == "Filename")]
  data_to_numeric_list = function(col) lapply(col, function(x) as.numeric(strsplit(x, "\\s+")[[1]]))
  
  df_no_filename = apply(df_no_filename, 2, data_to_numeric_list)
  df_no_filename$Filename = df$Filename
  return (df_no_filename)
}

