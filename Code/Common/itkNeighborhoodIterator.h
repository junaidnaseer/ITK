/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkNeighborhoodIterator.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

Copyright (c) 2001 Insight Consortium
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

 * The name of the Insight Consortium, nor the names of any consortium members,
   nor of any contributors, may be used to endorse or promote products derived
   from this software without specific prior written permission.

  * Modified source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS ``AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/
#ifndef __itkNeighborhoodIterator_h
#define __itkNeighborhoodIterator_h

#include <vector>
#include <string.h>
#include <iostream>
#include "itkImage.h"
#include "itkIndex.h"
#include "itkOffset.h"
#include "itkSize.h"
#include "itkImageRegion.h"
#include "itkMacro.h"
#include "itkNeighborhood.h"
#include "itkImageBoundaryCondition.h"

namespace itk {

/**
 * \class NeighborhoodIterator
 * \brief A virtual object that defines the basic API and common functionality
 * for all flavors of NeighborhoodIterators.
 *
 * NeighborhoodIterator is a virtual object that defines the API and 
 * common functions for all types of NeighborhoodIterators.  A
 * NeighborhoodIterator is a neighborhood of pointers to itk::Image pixels
 * that can be moved sequentially across the image.  A NeighborhoodIterator
 * can be "dereferenced" to obtain a Neighborhood of values.  Certain
 * Neighborhood operations on itk::Image pixel values can be applied directly
 * on a NeighborhoodIterator for efficiency.
 *
 * Dereferencing is handled differently by each subclass.
 * SmartRegionNeighborhoodIterator's, for example, perform bounds checking
 * and handle boundary conditions during dereferencing, while
 * RegionNeighborhoodIterator's do not.
 *
 * The API for creating and manipulating a NeighborhoodIterator mimics
 * that of the itk::ImageIterators.  Like the itk::ImageIterator, a
 * NeighborhoodIterator is defined on a region of interest in an itk::Image.
 * Iteration is constrained within that region of interest.  "Smart"
 * NeighborhoodIterators exist for handling boundary conditions and for
 * constraining iteration to the boundaries of a region.
 *
 * Each subclass of a NeighborhoodIterator also defines its own mechanism for
 * iteration through an image.  In general, the Iterator does not directly
 * keep track of its spatial location in the image, but uses a set of
 * internal loop variables and offsets to trigger wraps at itk::Image region
 * boundaries, and to identify the end of the itk::Image region.
 *
 * It is important to note that NeighborhoodIterators are "forward iterators".
 * They move only in one direction.  Furthermore, there is no guaranteed
 * path that a NeighborhoodIterator will follow across an image memory
 * buffer.  When a NeighborhoodIterator is incremented by one, for example,
 * it cannot be assumed that its internal pointers have been incremented by
 * one.
 *
 * There are two easy ways to spatially synchronize the output of
 * neighborhood operations with the movements of a NeighborhoodIterator:
 *
 * (1) A NeighborhoodIterator can be given a pointer to an output buffer that
 * is moved in synch with the center position of the NeighborhoodIterator.
 * The output buffer is assumed to be the same size as the itk::Image buffer
 * on which the NeighborhoodIterator is defined.  Scalar output can be written
 * directly to the synchronized pointer location.
 *
 * (2) Like NeighborhoodIterators are guaranteed to follow the same path
 * through itk::Image buffers with equal region sizes and region start
 * locations.  In other words, you can write output through a second
 * NeighborhoodIterator defined on the output buffer that you increment
 * in synch with the original.
 *
 * \sa Image
 * \sa Neighborhood
 * \sa ImageIterator
 * \sa RegionNeighborhoodIterator
 * \sa SmartRegionNeighborhoodIterator
 * \sa RegionBoundaryNeighborhoodIterator
 */

template<class TImage>
class ITK_EXPORT NeighborhoodIterator
  :  public Neighborhood<typename TImage::InternalPixelType *,
                                           TImage::ImageDimension>
{
public:
  /**
   * Extract image type information.
   */
  typedef typename TImage::InternalPixelType InternalPixelType;
  typedef typename TImage::PixelType PixelType;
  enum {Dimension = TImage::ImageDimension };
  
  /** 
   * Standard "Self" & Superclass typedef support.
   */
  typedef NeighborhoodIterator Self;
  typedef Neighborhood<InternalPixelType *, Dimension> Superclass;

  /**
   * Typedef support for common objects
   */
  typedef typename Superclass::SizeType SizeType;
  typedef TImage ImageType;
  typedef typename TImage::RegionType RegionType;
  typedef Index<Dimension> IndexType;
  typedef Offset<Dimension> OffsetType;
  typedef typename Superclass::RadiusType RadiusType;
  typedef Neighborhood<PixelType, Dimension> NeighborhoodType;
  
  /**
   * Support for internal iterator types
   */
  typedef typename Superclass::Iterator Iterator;
  typedef typename Superclass::ConstIterator ConstIterator;

  /**
   * Typedef for generic boundary condition pointer
   */
  typedef ImageBoundaryCondition<ImageType> *
   ImageBoundaryConditionPointerType;
  
  /**
   * Scalar data type typedef support
   */
  typedef typename ScalarTraits<PixelType>::ScalarValueType ScalarValueType;
  
  /**
   * Default constructor.
   */
  NeighborhoodIterator();
  
  /**
   * Copy constructor
   */
  NeighborhoodIterator( const NeighborhoodIterator & );
  
  /**
   * Assignment operator
   */
  Self &operator=(const Self& orig);
  
 
  /**
   * Constructor which establishes the region size, neighborhood, and image
   * over which to walk.
   */
  NeighborhoodIterator(const SizeType &radius,
                       ImageType * ptr,
                       const RegionType &region
                       )
    { this->Initialize(radius, ptr, region);  }

  /**
   * Standard print method
   */
  virtual void PrintSelf(std::ostream &, Indent) const;
  
  /**
   * Initializes the iterator to walk a particular image and a particular
   * region of that image.
   */
  virtual void Initialize(const SizeType &radius, ImageType *ptr,
                          const RegionType &region);


  /**
   * Computes the internal, N-d offset of a pixel array position n from 
   * (0,0, ..., 0) in the "upper-left" corner of the neighborhood.
   */
  OffsetType ComputeInternalIndex(unsigned int n) const;
  
  /**
   * Increments the pointers in the NeighborhoodIterator,
   * wraps across boundaries automatically, accounting for
   * the disparity in the buffer size and the region size of the
   * image.
   *
   * This function may be overridden by a subclass.
   */
  virtual const Self &operator++();
  
  /**
   * Decrements the pointers in the NeighborhoodIterator,
   * wraps across boundaries automatically, accounting for
   * the disparity in the buffer size and the region size of the
   * image.
   *
   * This function may be overridden by a subclass.
   */
  virtual const Self &operator--();  
  
  /**
   * Virtual function that replaces the pixel values in the image
   * neighborhood "pointed to" by the NeighborhoodIterator with
   * the pixel values contained in a Neighborhood.
   *
   * This method must be defined by each subclass according to how
   * that subclass handles boundary conditions.
   */
  virtual void SetNeighborhood(NeighborhoodType &) = 0;

  /**
   * Virtual function that "dereferences" a NeighborhoodIterator,
   * returning a Neighborhood of pixel values.
   *
   * This function must be implemented separately by each
   * subclass according to how that subclass handles boundary
   * conditions.
   */
  virtual NeighborhoodType GetNeighborhood() const = 0;
  
  /**
   * Returns the pixel value referenced at a linear array location.
   */
  virtual PixelType GetPixel(const unsigned long i) const
  {  return *(this->operator[](i));  }

  /**
   * Returns the offsets used to wrap across dimensional boundaries.
   */
  OffsetType GetWrapOffset() const
  {  return m_WrapOffsets;  }
  
  /**
   * Sets the offsets that will be used to adjust for any differences in input
   * and output buffer sizes.
   */
  void SetOutputWrapOffsetModifier( const OffsetType &modifiers);
 
  /**
   * Returns the offsets that will be used to adjust for differences in input
   * and output buffer sizes.
   */
  OffsetType GetOutputWrapOffsetModifier() const
  {  return m_OutputWrapOffsetModifier;  }
  
  /**
   * Returns the internal offset associated with wrapping around a single
   * dimension's region boundary in the itk::Image.  An offset for each
   * dimension is necessary to shift pointers when wrapping around region
   * edges because region memory is not necessarily contiguous within the
   * buffer.
   */
  long GetWrapOffset(unsigned int n) const
  {    return m_WrapOffsets[n];   }

  /**
   * Returns a const pointer to an internal array of upper loop bounds used
   * during iteration.
   */
  IndexType GetBound() const
  {    return m_Bound;   }

  /**
   * Returns the internal loop bound used to define the edge of a single
   * dimension in the itk::Image region.
   */
  long GetBound(unsigned int n) const
  {    return m_Bound[n];  }

  /**
   * Returns the N-dimensional index of the iterator's position in
   * the image.
   */
  virtual IndexType GetIndex() const
  { return m_Loop;  }

  /**
   * Returns the N-dimensional starting index of the iterator's position on
   * the image.
   */
  IndexType GetStartIndex() const
  { return m_StartIndex; }
  
  /**
   * Returns a boolean == comparison of the memory addresses of the center
   * elements of two NeighborhoodIterators of like pixel type and
   * dimensionality.  The radii of the iterators are ignored.
   */
  bool operator==(const Self &it) const 
  {    return  it[this->Size()>>1] == (*this)[this->Size()>>1];   }
  
  /**
   * Returns a boolean != comparison of the memory addresses of the center
   * elements of two NeighborhoodIterators of like pixel type and
   * dimensionality.  The radii of the iterators are ignored.
   */
  bool operator!=(const Self &it) const
  {    return  it[this->Size()>>1] != (*this)[this->Size()>>1];  }
  
  /**
   * Returns a boolean < comparison of the memory addresses of the center
   * elements of two NeighborhoodIterators of like pixel type and
   * dimensionality.  The radii of the iterators are ignored.
   */
  bool operator<(const Self &it) const
  {  return  (*this)[this->Size()>>1] < it[this->Size()>>1];  }

  /**
   * Returns a boolean < comparison of the memory addresses of the center
   * elements of two NeighborhoodIterators of like pixel type and
   * dimensionality.  The radii of the iterators are ignored.
   */
  bool operator<=(const Self &it) const
  {    return  (*this)[this->Size()>>1] <= it[this->Size()>>1];  }
  
  /**
   * Returns a boolean > comparison of the memory addresses of the center
   * elements of two NeighborhoodIterators of like pixel type and
   * dimensionality.  The radii of the iterators are ignored.
   */
  bool operator>(const Self &it) const
  {    return  (*this)[this->Size()>>1] > it[this->Size()>>1];  }

  /**
   * Returns a boolean >= comparison of the memory addresses of the center
   * elements of two NeighborhoodIterators of like pixel type and
   * dimensionality.  The radii of the iterators are ignored.
   */
  bool operator>=(const Self &it) const
  {    return  (*this)[this->Size()>>1] >= it[this->Size()>>1];  }

  /**
   * Returns the pixel referenced at the center of the NeighborhoodIterator.
   */
  PixelType Center() const
  {    return *(this->operator[]((this->Size())>>1));  }

  /**
   * Returns the central memory pointer of the neighborhood.
   */
  InternalPixelType *CenterPointer() const
  {    return (this->operator[]((this->Size())>>1));  }
  
  /**
   * "Scalar" dereference.  References the pixel on which the iterator is
   * centered.  Allows a NeighborhoodIterator to be used in the context of a
   * regular image iterator.
   */
  //  PixelType &operator*() const
  //  {    return *(this->CenterPointer());  }
  
  /**
   * Sets the internal pointer to a memory buffer that is incremented
   * in synch with the center of the NeighborhoodIterator.  This
   * internal pointer can be used to guarantees spatial fidelity between
   * inputs and outputs to an algorithm that uses NeighborhoodIterators.
   * \sa GetOutputBuffer
   */
  void SetOutputBuffer(InternalPixelType *i)
  {    m_OutputBuffer = i;   }

  /**
   * Returns the current memory location of the internal output
   * pointer that is synchronized with the iterator.
   * \sa SetOutputBuffer
   */
  InternalPixelType *GetOutputBuffer() const
  {    return m_OutputBuffer;  }
  
  /**
   * This method positions the iterator at an indexed location in the
   * image. SetLocation should _NOT_ be used to update the position of the
   * iterator during iteration, only for initializing it to a position
   * prior to iteration.  This method is not optimized for speed.
   */
  void SetLocation( const IndexType& position )
  {
    this->SetLoop(position);
    this->SetPixelPointers(position);
  }

  /**
   * Returns a smartpointer to the image on which this iterator operates.
   */
  typename ImageType::Pointer GetImagePointer() { return m_Image; }

  /**
   * Virtual method for rewinding the iterator to its beginning pixel.
   * This is useful for writing functions which take neighborhood iterators
   * of arbitrary type and must use virtual functions.
   */
  virtual void SetToBegin() = 0;

  /**
   * Virtual method for determining whether the the iterator has reached the
   * end of its iteration region.  Useful for writing functions that take
   * neighborhood iterators of arbitrary type and must use virtual functions.
   */
  virtual bool IsAtEnd() const
  {    return ( this->operator[](this->Size()>>1) == m_EndPointer );   }

  /**
   * Sets the EndPointer as appropriate for the particular iterator type.
   */
  virtual void SetEnd() = 0;

  /**
   *  Returns the region of iteration.
   */
  RegionType GetRegion() const
  { return m_Region; }

  /**
   * Allows a user to override the internal boundary condition. Care should be
   * be taken to ensure that the overriding boundary condition is a persistent
   * object during the time it is referenced.  The overriding condition
   * can be of a different type than the default type as long as it is
   * a subclass of ImageBoundaryCondition.
   *
   * This method is only relevant in iterators that have the capability
   * to handle boundary conditions.
   */
  virtual void OverrideBoundaryCondition(const ImageBoundaryConditionPointerType i)
  { /* default case is do nothing */ }

  /**
   * Resets the boundary condition to the internal, default conditions
   * specified by the template parameter.
   */
  virtual void ResetBoundaryCondition()
  { /* default case is do nothing */ }
 
protected:
  /**
   * Computes the internal table of stride lengths.
   */
  void ComputeStrideTable()
    {
      for (unsigned int i = 0; i < Dimension; ++i)
        { m_StrideTable[i] = this->GetStride(i); }
    }
  
  /**
   * Default method for setting the coordinate location of the iterator.
   * Loop indicies correspond to the actual Image region index.
   * This correspondence is a coincidental feature that will not
   * necessarily be supported.
   */
  virtual void SetLoop( const IndexType& p )
  {  m_Loop = p; }
  
  /**
   * Default method for setting the index of the first pixel in the
   * iteration region.
   */
  virtual void SetStartIndex( const IndexType& start)
  {  m_StartIndex = start;  }
  
  /**
   * Virtual method for setting internal loop boundaries.  This
   * method must be defined in each subclass because
   * each subclass may handle loop boundaries differently.
   */
  virtual void SetBound(const SizeType &) = 0;

  /**
   * Default method for setting the values of the internal pointers
   * to itk::Image memory buffer locations.  This method should
   * generally only be called when the iterator is initialized.
   * \sa SetLocation
   */
  virtual void SetPixelPointers(const IndexType &);

  /**
   * The region over which iteration is defined.
   */
  RegionType m_Region;
  
  /**
   * The starting index for iteration within the itk::Image region
   * on which this NeighborhoodIterator is defined.
   */
  IndexType m_StartIndex;

  /**
   * Array of loop counters used during iteration.
   */
  IndexType m_Loop;

  /**
   * An array of upper looping boundaries used during iteration.
   */
  IndexType m_Bound;

  /**
   * The internal array of offsets that provide support for regions of interest.
   * An offset for each dimension is necessary to shift pointers when wrapping
   * around region edges because region memory is not necessarily contiguous
   * within the buffer.
   */
  OffsetType m_WrapOffset;
  
  /**
   * Modifiers that compensate for the difference in the sizes of the input
   * and output buffers.  m_OutputBuffer will be incremented by these
   * additional values when wrapping across dimensional boundaries during
   * iteration.
   */
  OffsetType m_OutputWrapOffsetModifier;
  
  /**
   * A pointer to an output buffer that, if set, is moved in synch with
   * the center position of the NeighborhoodIterator.
   *
   * The output buffer is assumed to be the same size as the itk::Image buffer
   * on which the NeighborhoodIterator is defined.  Scalar output can be
   * written directly to the synchronized pointer location. 
   */
  InternalPixelType *m_OutputBuffer;

  /**
   * The pointer to the itk::Image on which this NeighborhoodIterator is
   * defined.
   */
  typename ImageType::Pointer m_Image;

  /**
   * Shortcut to the data buffer of the itk::Image on which this
   * NeighborhoodIterator is defined.
   */
  InternalPixelType *m_Buffer;

  /**
   *
   */
  InternalPixelType *m_EndPointer;

  /**
   * A table containing the stride length (step size in memory units)
   * for each dimension.  Computed once at construction.
   */
  unsigned long m_StrideTable[Dimension];
  
};

} // namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkNeighborhoodIterator.txx"
#endif

#endif
