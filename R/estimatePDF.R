estimatePDF <- function(sample, pdfLength = NULL, estimationPoints = NULL,
                        lowerBound = NULL, upperBound = NULL, target = 70, 
                        lagrangeMin = 1, lagrangeMax = 200, 
                        debug = 0, outlierCutoff = 7) {

	inputLength = vector("numeric", 1)
	inputLength [1] = length(sample)

	if (is.null(pdfLength)) {
	  pdfLength = floor(200 + inputLength/200.0)
	  if (pdfLength > 1500) {
	    pdfLength = 1500;	
	  }
	}
	

	estimationLength = vector("numeric", 1)
	isSpecifyPoints = vector("numeric", 1)
	if (is.null(estimationPoints)) {
	  isSpecifyPoints[1] = FALSE
	} else {
	  isSpecifyPoints[1] = TRUE
	  estimationLength[1] = length(estimationPoints)
	  if (length(estimationPoints) > pdfLength) {
	    pdfLength = length(estimationPoints)
	  }
	}
	
	outputLength = vector("numeric", 1)
	outputLength[1] = pdfLength
	
	low = vector("numeric", 1)
	high = vector("numeric", 1)
	
	if (is.null(lowerBound)) {
		isLow = vector("numeric", 1)
		isLow[1] = FALSE
	} else {		
		low[1] = lowerBound
		isLow = vector("numeric", 1)
		isLow[1] = TRUE
	}
	if (is.null(upperBound)) {
		isHigh = vector("numeric", 1)
		isHigh[1] = FALSE
	} else {		
		high[1] = upperBound
		isHigh = vector("numeric", 1)
		isHigh[1] = TRUE
	}

	
	failedSolution = vector("numeric", 1)
	failedSolution [1] = 0
	
	threshold = vector("numeric", 1)
	threshold [1] = 0

	sqrSize = vector("numeric", 1)
	sqrSize [1] = 0
	

	distribution=.C(getNativeSymbolInfo("estimatePDF", "PDFEstimator"), 
				sample = as.double(sample), 
				inputLength = as.integer(inputLength),
				estimationPoints = as.double(estimationPoints),
				estimationLength = as.integer(estimationLength),
				isSpecifyPoints = as.integer(isSpecifyPoints),
				low = as.double(low), 	
				high = as.double(high), 
				isLow = as.integer(isLow), 
				isHigh = as.integer(isHigh),
				target = as.double(target),
				lagrangeMin = as.integer(lagrangeMin),
				lagrangeMax = as.integer(lagrangeMax),
				debug = as.integer(debug),
				outlierCutoff = as.integer(outlierCutoff),
				outputLength = as.integer(outputLength),
				failedSolution = as.integer(failedSolution),
				threshold = as.double(threshold),
				x = as.double(vector("numeric", outputLength)), 
				pdf = as.double(vector("numeric", outputLength)), 
				cdf = as.double(vector("numeric", outputLength)),
				sqr = as.double(vector("numeric", inputLength)),
				sqrSize = as.double(sqrSize),
				lagrange = as.double(vector("numeric", lagrangeMax)),
				r = as.double(vector("numeric", inputLength)))
	
	      if (!is.null(estimationPoints)) {
	        distribution$x = distribution$x[1:estimationLength]
	        distribution$pdf = distribution$pdf[1:estimationLength]
	        distribution$cdf = distribution$cdf[1:estimationLength]
	      }
	
	      class(distribution) <- "PDFe"
	      return(distribution)
}
