\name{getTarget}
\alias{getTarget}
\title{Define Target Outliers}
\description{calculates position-dependent threshold values about the mean according to a beta distribution with parameters k and (n + 1 - k), where k is the position and n is the total number of positions.  These beta distributions represent probability per position for sort order statistics for a uniform distribution.}
\usage{getTarget(Ns, target)}
\arguments{
  \item{Ns}{number of samples}
  \item{target}{target confidence threshold}
}
\details{plotTarget is intended for use with the plot method in the PDFEstimator package for plotting PDFe density estimation objects.
}
\value{
\item{target}{a two dimensional matrix defining the  upper and lower confidence thresholds for the requested target.}
}
\references{Farmer, J. and D. Jacobs (2018). "High throughput nonparametric probability density estimation." PloS one 13(5): e0196937.}
\author{Jenny Farmer, Donald Jacobs}
\seealso{plot.PDFe}
