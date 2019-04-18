\name{estimatePDF}
\alias{estimatePDF}
\title{Nonparametric Density Estimation}
\description{Estimates the probability density function for a data sample.}
\usage{estimatePDF(sample, pdfLength = NULL, lowerBound = NULL, upperBound = NULL)}
\arguments{
	\item{sample}{the data sample from which to calculate the density estimate}	
	\item{pdfLength}{the desired length of the estimate returned.  Default value is calculated based on sample length.  Overriding this calculation can increase or decrease the resolution of the estimate.}
	\item{lowerBound}{the lower bound of the PDF, if known.  Default value is calculated based on the range of the data sample.}
	\item{upperBound}{the upper bound of the PDF, if known.  Default value is calculated based on the range of the data sample.}
}
\details{
A nonparametric density estimator based on the maximum-entropy method.  Accurately predicts a probability density function (PDF) for random data using a novel iterative scoring function to determine the best fit without overfitting to the sample.
}
\value{
	\item{x}{estimated range of density data}
	\item{pdf}{estimated probability density function}
	\item{cdf}{estimated cummulative density function}
	\item{sqr}{scaled quantile residual.  Provides a sample-size invariant measure of the fluctuations in the estimate.}
	\item{lagrange}{lagrange multipliers.  Can be used to reproduce the expansions for an analytical solution.}
	\item{failedSolution}{returns true if the pdf calculated is not considered an acceptable estimate of the data according to the scoring function.}
}
\references{Farmer, J. and D. Jacobs (2018). "High throughput nonparametric probability density estimation." PloS one 13(5): e0196937.}
\author{Jenny Farmer, Donald Jacobs}
\examples{
#Estimates a normal distribution with 100 sample points using default parameters

sampleSize = 1000
sample = rnorm(sampleSize, 0, 1)
dist = estimatePDF(sample)
plot(dist$x, dist$pdf)

}
