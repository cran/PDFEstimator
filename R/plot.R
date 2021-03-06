plot.PDFe <- function(x, plotPDF = TRUE, plotSQR = FALSE, 
                      plotShading = FALSE, shadeResolution = 100, 
                      showOutlierPercent = 0, outlierColor = "red3", 
                      sqrColor = "steelblue4", type="l", lwd = 2, ...){
  estimate = x
  x = estimate$x
  pdf = estimate$pdf
  sqr = estimate$sqr[1:estimate$sqrSize]
  sample = estimate$sample
  mean = seq(0, 1, 1/(length(sqr) - 1))
  
  if (showOutlierPercent) {
    targets = suppressWarnings(getTarget(length(sqr), showOutlierPercent))
    above = sqr > targets[ , 1]
    below = sqr < targets[ , 2]
    outside = above | below
    iOutside = which(outside == TRUE)
    nOutside = length(iOutside)
  }
  
  if (plotShading) {
    if (plotPDF) {
      y = sort(sample)
      y = y[y >= min(x) & y <= max(x)]
      plotBeta(y, xPlotRange = range(x), resolution = shadeResolution)
    } else {
      plotBeta(mean, xPlotRange = range(mean), resolution = shadeResolution)
    }
    par(new = TRUE)
  }
  
  if (plotSQR && plotPDF) {
    xMin = max(min(x), min(sample))
    xMax = min(max(x), max(sample))
    y = sort(sample)
    if (length(sample) > length(sqr)) {
      inRange = which((y > min(x)) & (y < max(x)))
      y = y[inRange]
    }
    if (length(sample) == length(sqr)) {
      plot(y, sqr, type = "l", ylim = c(-2, 2), xlim = range(x), axes = FALSE, col = sqrColor, xlab ="", ylab = "")
      axis(4, ylim = c(-2, 2), ylab = "sqr", col = sqrColor, col.ticks = sqrColor, col.axis = sqrColor)
      par(new = TRUE)
      if (showOutlierPercent) {
        if (nOutside > 0) {
          symbols(y[iOutside], sqr[iOutside], add = TRUE, circles = rep(0.01, length(iOutside)), bg = outlierColor, fg = outlierColor, inches = 0.01, xlab ="", ylab = "", ylim = c(-2, 2), xlim = c(0, 1))
          par(new = TRUE)
        }
      }
    }
  }
  
  if (plotSQR && !plotPDF) {
    plot(mean, sqr, ylim = c(-2, 2), type = type, lwd = lwd, col = sqrColor, ...)
    par(new = TRUE)
    if (showOutlierPercent) {
      plot(mean, targets[ , 1], type = "l", lty = 2, ylim = c(-2, 2), xlab = "", ylab = "")
      par(new = TRUE)
      plot(mean, targets[ , 2], type = "l", lty = 2, ylim = c(-2, 2), xlab = "", ylab = "")
      par(new = TRUE)
      percentActual = ceiling(nOutside / length(sqr) * 100)
      legend("bottomright", inset = 0.01, horiz = TRUE, legend = paste("outliers ", percentActual, "%", sep = ""), col = outlierColor, lty=1, cex=0.6, box.lty = 0)
      legend("topright", inset = 0.01, horiz = TRUE, legend = paste("confidence threshold ", showOutlierPercent, "%", sep = ""), col = sqrColor, lty=2, cex=0.6, box.lty = 0)
      if (nOutside > 0) {
        symbols(mean[iOutside], sqr[iOutside], add = TRUE, circles = rep(0.01, length(iOutside)), bg = outlierColor, fg = outlierColor, inches = 0.01, xlab ="", ylab = "", ylim = c(-2, 2), xlim = c(0, 1))
        par(new = TRUE)
      }
    }
  }
  
  if (plotPDF) {
    plot.default(x, pdf, type = type, lwd = lwd, ...)
    if (!plotSQR && showOutlierPercent) {
      if (nOutside > 0) {
        xMin = max(min(x), min(sample))
        xMax = min(max(x), max(sample))
        d = seq(xMin, xMax, length.out = length(sqr))
        outliers = approx(x, pdf, xout = d[iOutside])
        symbols(outliers$x, outliers$y, add = TRUE, circles = rep(0.01, length(iOutside)), bg = outlierColor, fg = outlierColor, inches = 0.01, xlab ="", ylab = "", ylim = c(-2, 2), xlim = c(0, 1))
      }
    }
  }
  
  par(new = FALSE)
  
}