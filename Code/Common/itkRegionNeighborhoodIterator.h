/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkRegionNeighborhoodIterator.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) 2000 National Library of Medicine
  All rights reserved.

  See COPYRIGHT.txt for copyright details.

=========================================================================*/
#ifndef __itkRegionNeighborhoodIterator_h
#define __itkRegionNeighborhoodIterator_h

#include "itkNeighborhoodIterator.h"

namespace itk {

/**
 * \class RegionNeighborhoodIterator
 * \brief Subclass of NeighborhoodIterator designed for fast iteration
 * over an itk::Image region of interest that does no bounds checking.
 *
 * RegionNeighborhoodIterator is the simplest of the NeighborhoodIterators.
 * It is optimized for fast iteration of a pixel neighborhood over an
 * image.  It performs no bounds checking and does not handle boundary
 * conditions on dereferencing.
 *
 * RegionNeighborhoodIterator only maintains counters for loop position and
 * upper bounds, and so it is "unaware" when it is overlapping a region
 * boundary.
 * You can only safely use this iterator on regions sufficiently contained
 * within the itk::Image buffer.
 *
 * \sa NeighborhoodIterator
 * \sa RegionBoundaryNeighborhoodIterator
 * \sa SmartRegionNeighborhoodIterator
 * \sa Neighborhood
 * \sa NeighborhoodAlgorithm
 */
 
template<class TImage,
  class TAllocator =
   NeighborhoodAllocator<ImageTraits<TImage>::InternalPixelType *>,
  class TDerefAllocator =
   NeighborhoodAllocator<ImageTraits<TImage>::PixelType>
  >
class ITK_EXPORT RegionNeighborhoodIterator
 : public NeighborhoodIterator<TImage, TAllocator, TDerefAllocator>
{
public:
  /** 
   * Standard "Self" & Superclass typedef support.
   */
  typedef RegionNeighborhoodIterator Self;
  typedef NeighborhoodIterator<TImage, TAllocator, TDerefAllocator> Superclass;

  /**
   * Extract image type information.
   */
  typedef typename Superclass::InternalPixelType InternalPixelType;
  typedef typename Superclass::PixelType PixelType;
  enum {Dimension = Superclass::Dimension };
  
  /**
   * Some common itk object typedefs
   */
  typedef typename Superclass::ImageType ImageType;
  typedef typename Superclass::RegionType RegionType;
  typedef typename Superclass::SizeType SizeType;
  typedef typename Superclass::NeighborhoodType NeighborhoodType;
  typedef typename Superclass::IndexType IndexType;

  /**
   * Scalar data type typedef support
   */
  typedef typename Superclass::ScalarValueType ScalarValueType;

  /**
   * Default constructor
   */
  RegionNeighborhoodIterator() {};

  /**
  * Constructor establishes a neighborhood of iterators of a specified
  * dimension to walk a particular image and a particular region of
  * that image.
  */
  RegionNeighborhoodIterator(const SizeType &radius,
                             ImageType *ptr,
                             const RegionType &region)
  {  this->Initialize(radius, ptr, region);   }

  /**
   * Return an iterator for the beginning of the region.
   */
  Self Begin() const;

  /**
   * Return an iterator for the end of the region.
   */
  Self End() const; 

  /**
   * 
   */
  void SetEnd()    
  {  m_EndPointer = this->End().operator[](this->Size()>>1);  }
  
  /**
   *
   */
  void SetToBegin()
  {  *this = this->Begin();  }
  
  /**
   * Returns a Neighborhood object with values of the image pixels that
   * are referenced by the NeighborhoodIterator's internal pointers.
   * \sa SetPixelValues
   * \sa Neighborhood
   */
  NeighborhoodType GetNeighborhood();

  /**
   * Sets the values in the referenced image to the values contained in
   * a Neighborhood object.  This method assumes that the Neighborhood object
   * argument and the NeighborhoodPointer are equal size.
   */
  void SetNeighborhood(NeighborhoodType &);

  /**
   * Copy constructor
   */
  RegionNeighborhoodIterator( const Self& other)
    : NeighborhoodIterator<TImage, TAllocator, TDerefAllocator>(other)
  {  }

  /**
   * Assignment operator
   */
  Self &operator=(const Self& orig)
  {
    Superclass::operator=(orig);
    return *this;
  }

  /**
   * Standard print method.
   */
  virtual void PrintSelf(std::ostream &os, Indent i) const
  {
    os << i << "RegionNeighborhoodIterator" << std::endl;
    Superclass::PrintSelf(os, i.GetNextIndent());
  }
  
protected:
  /**
   * Sets the loop upper boundaries for iteration.
   */
  void SetBound(const SizeType &);

};
  
} // namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkRegionNeighborhoodIterator.txx"
#endif

#endif 

