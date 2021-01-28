\name{plotBeta}
\alias{plotBeta}
\title{Plot Diagnostic Shading}
\description{Plot background shading for density estimation based on the beta distribution for sort order statistics}
\usage{plotBeta(samples, resolution = 100, xPlotRange)}
\arguments{
  \item{samples}{a data sample for estimation}
  \item{resolution}{the number of sample points plotted in the contour}
  \item{xPlotRange}{the x-axis range for plotting}
}
\details{plotBeta is intended for use with the plot method in the PDFEstimator package for plotting PDFe density estimation objects.
}
\references{
Farmer, J. and D. Jacobs (2018). "High throughput nonparametric probability density estimation." PloS one 13(5): e0196937.
}
\author{Jenny Farmer, Donald Jacobs}
\seealso{plot.PDFe}

