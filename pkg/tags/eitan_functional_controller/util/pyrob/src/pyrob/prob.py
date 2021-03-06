# Copyright (c) 2008, Willow Garage, Inc.
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of the Willow Garage, Inc. nor the names of its
#       contributors may be used to endorse or promote products derived from
#       this software without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
## @author Hai Nguyen/hai@gatech.edu
# Probability Utility Functions
import numpy
import fun
from itertools import *
from numpy.linalg.linalg import det
from numpy.linalg.linalg import inv
from StringIO import StringIO 

def cov(m):
   """
   Returns an estimate of the covariance matrix associated with the data m.
   m is a dxn matrix where d is the number of dimensions and n is the number of points
   """
   if (m.__class__ != numpy.matrix):
      raise AssertionError
   num_el = m.shape[1]
   mean   = m.mean(1)
   #msub   = m - numpy.tile(mean, (1, num_el))
   msub   = (m.T - mean.A[:,0]).T
   return (msub * msub.T) / (num_el - 1)


def _weight(mat, weights):
   """
   Deprecated: This is an inefficient implementation.
   Returns the result of mat*eye(weights).
   """
   print "prob._weight: WARNING (Deprecated) This is an inefficient implementation."
   for i in xrange(mat.shape[1]):
      mat[:,i] = mat[:,i] * weights[0,i]
   return mat


def cov_w(mat, weights):
   """
   Returns a weighted covariance matrix.
   """
   if (mat.__class__ != numpy.matrix):
      raise AssertionError
   num_el = mat.shape[1]
   total_weights = numpy.sum(weights)
   #mat_w         = _weight(mat.copy(), weights)
   mat_w         = numpy.matrix(mat.A * weights.A)
   mean          = numpy.sum(mat_w, axis=1) / total_weights

   #tiled_mean    = numpy.tile(mean, (1, num_el))
   #m_sub         = mat - tiled_mean
   m_sub         = (mat.T - mean.A[:,0]).T
   m_sub_t       = m_sub.T 
   #result = (_weight(m_sub, weights) * m_sub_t) / (total_weights - 1)
   result = (numpy.matrix(m_sub.A * weights.A) * m_sub_t) / (total_weights - 1)
   return result


def fit(points):
   """
   Returns a Gaussian object fit to input points.
   """
   if (points.__class__ != numpy.matrix):
      raise RuntimeError("Param points is not of type matrix")
   mean = points.mean(1)
   cov = cov(points)
   return Gaussian(mean, cov)


def fit(points, weights):
   """
   Returns a Gaussian object fit to the weighted input points.
   """
   if (points.__class__ != numpy.matrix):
      raise RuntimeError("Param points is not of type matrix")
   #def wmul(el): 
   #   x,w = el
   #   return x*w
   #paired = izip(fun.points_of_mat(points), fun.points_of_mat(weights))
   #mean0 = reduce(lambda x, y: x+y, imap(wmul, paired)) / weights.sum()
   mean = (points * numpy.matrix(weights.A[0,:]).T)/weights.sum()
   mean = numpy.matrix(mean).T
   cov  = cov_w(points, weights)
   return Gaussian(mean, cov)


class Gaussian(object):
    """ 
        Class for multidimensional Gaussians 
        TODO: make m & v optional
    """

    def __init__(self, m, v, dimension=None):
       if dimension is not None:
          #dx1 where d is the number of dimensions
          self.mean = numpy.matrix(numpy.zeros((dimension,1)))
          #dxd
          self.cov  = numpy.matrix(numpy.eye(dimension))
       else:
# forcibly make the mean a NX1 column vector - Advait (for numpy 1.0.3)
          sort_shape = numpy.sort(m.shape)
          self.mean = numpy.matrix(m).reshape(sort_shape[1], 1)
#          self.mean = m
          self.cov = v


    def pdf(self):
       """ Partially applied Gaussian pdf """
       dim     = self.mean.shape[0]
       const   = 1 / (((2*numpy.pi)**(dim/2)) * (det(self.cov)**0.5))
       inv_cov = inv(self.cov)
       def gauss_pdf(x):
          sub = x - self.mean
          exponent = -0.5* sub.T * inv_cov * sub
          if (numpy.shape(exponent) != (1,1)):
             raise AssertionError
          return const * (numpy.e ** exponent[0,0])
       return gauss_pdf  


    def pdf_mat(self):
       """ Return a partially applied Gaussian pdf that takes in a matrix whose columns are the input vectors"""
       dim     = self.mean.shape[0]
       const   = 1 / (((2*numpy.pi)**(dim/2)) * (det(self.cov)**0.5))
       inv_cov = inv(self.cov)
       def gauss_pdf_mat(x):
          """Partially applied Gaussian pdf that takes in a matrix whose columns are the input vectors"""
          sub = x - self.mean
          r0 = inv_cov * sub
          exponent = -0.5 * numpy.sum(sub.A * r0.A, axis=0)
          if (numpy.shape(exponent) != (x.shape[1],)):
             raise AssertionError("exponent has the wrong shape, should be (%d,), but is (%d,)" % x.shape[1], exponent.shape[0])
          g = const * (numpy.e ** exponent)
          return g
       return gauss_pdf_mat


    def sample(self):
       """
       Returns a sample drawn from the Gaussian pdf.
       """
       return numpy.matrix(numpy.random.multivariate_normal(self.mean.A[0], self.cov.A)).T

    def __sub__(self, other):
       """
       Compare gaussians using bootlegged frobenius norm
       """
       mean_diff = numpy.linalg.norm(self.mean - other.mean)
       cov_diff  = numpy.linalg.norm(self.cov - other.cov)
       return mean_diff + cov_diff

    def __str__(self):
       """
       Prints the parameters associated with the Gaussian.
       """
       p = StringIO()
       print >>p, "(mean = ", self.mean.T, ", \n     cov = \n", self.cov, ")", 
       s = p.getvalue()
       return s
