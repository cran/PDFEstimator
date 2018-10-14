estimatePDF <- function(sample, pdfLength = NULL, lowerBound = NULL, upperBound = NULL) {
	
	inputLength = vector("numeric", 1)
	inputLength [1] = length(sample)

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

	if (is.null(pdfLength)) {
		pdfLength = floor(200 + inputLength/200.0)
      	if (pdfLength > 1500) {
			pdfLength = 1500;	
		}
	}

	outputLength = vector("numeric", 1)
	outputLength[1] = pdfLength

	failedSolution = vector("numeric", 1)
	failedSolution [1] = 0


	distribution=.C("estimatePDF", 
				sample = as.double(sample), 
				inputLength = as.integer(inputLength), 
				low = as.double(low), 	
				high = as.double(high), 
				isLow = as.integer(isLow), 
				isHigh = as.integer(isHigh), 
				outputLength = as.integer(outputLength),
				failedSolution = as.integer(failedSolution), 
				x = as.double(vector("numeric", outputLength)), 
				pdf = as.double(vector("numeric", outputLength)), 
				cdf = as.double(vector("numeric", outputLength)),
				sqr = as.double(vector("numeric", outputLength)),
				lagrange = as.double(vector("numeric", 200)))
}