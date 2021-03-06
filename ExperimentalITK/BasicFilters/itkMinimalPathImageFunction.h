/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkMinimalPathImageFunction.h,v $
  Language:  C++
  Date:      $Date: 2009/03/21 01:33:09 $
  Version:   $Revision: 1.1 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkMinimalPathImageFunction_h
#define __itkMinimalPathImageFunction_h

#include "itkImageFunction.h"

#include "itkGradientImageFilter.h"
#include "itkPolyLineParametricPath.h"
#include "itkPriorityQueueContainer.h"

#include <vector>
#include <queue>

namespace itk
{

/** \class MinimalPathImageFunction
 * \brief Implements livewire image segmentation of Barret and Mortensen.
 *
 * MinimalPathImageFunction implements the livewire segmentation
 * algorithm of Barrett and Mortenson.  This class naturally derives
 * from the ImageFunction class where an N-D dimensional image
 * is taken as input and the output consists of a path in that image.
 *
 * \reference
 * W. A. Barrett and E. N. Mortenson, "Interactive live-wire boundary
 * extraction", Medical Image Analysis, 1(4):331-341, 1996/7.
 * \ingroup ImageFunctions
 */
template <class TInputImage, class TOutputType = PolyLineParametricPath<
  TInputImage::ImageDimension> >
class ITK_EXPORT MinimalPathImageFunction
: public ImageFunction<TInputImage, TOutputType>
{

public:
  /** ImageDimension constants */
  itkStaticConstMacro( ImageDimension, unsigned int,
                       TInputImage::ImageDimension );

  /** Standard class typedefs. */
  typedef MinimalPathImageFunction                         Self;

//  typedef PolyLineParametricPath<
//    itkGetStaticConstMacro( ImageDimension )>              TOutputType;
  typedef ImageFunction<TInputImage, TOutputType>                          Superclass;

  typedef SmartPointer<Self>                               Pointer;
  typedef SmartPointer<const Self>                         ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro( MinimalPathImageFunction, ImageFunction );

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Some convenient typedefs. */
  typedef TInputImage                                       InputImageType;
  typedef typename InputImageType::Pointer                  InputImagePointer;
  typedef typename InputImageType::ConstPointer             InputImageConstPointer;
  typedef typename InputImageType::RegionType               InputImageRegionType;
  typedef typename InputImageType::PixelType                InputImagePixelType;

  typedef typename Superclass::IndexType                    IndexType;
  typedef typename Superclass::PointType                    PointType;
  typedef typename Superclass::ContinuousIndexType          ContinuousIndexType;

  typedef typename TOutputType::VertexType                  VertexType;
  typedef typename TOutputType::Pointer                     OutputType;

  typedef float                                             RealType;
  typedef Image<RealType, ImageDimension>                   RealImageType;
  typedef GradientImageFilter<InputImageType, RealType,
    RealType>                                               GradientFilterType;
  typedef typename GradientFilterType::OutputImageType      GradientImageType;
  typedef Image<typename InputImageType::OffsetType,
    itkGetStaticConstMacro( ImageDimension )>               OffsetImageType;
  typedef Image<int,
    itkGetStaticConstMacro( ImageDimension )>               MaskImageType;
  typedef typename MaskImageType::PixelType                 MaskPixelType;


  /**
   * Priority queue typedefs
   */
  typedef MinPriorityQueueElementWrapper
    <IndexType, RealType>                            PriorityQueueElementType;
  typedef PriorityQueueContainer<
    PriorityQueueElementType,
    PriorityQueueElementType,
    RealType, long>                                  PriorityQueueType;

  virtual void SetInputImage( const InputImageType *ptr );

  /** Evaluate the function at specified Point position. */
  virtual typename TOutputType::Pointer Evaluate( const PointType &point ) const
				{
						IndexType index;
      this->ConvertPointToNearestIndex( point, index );
						return this->EvaluateAtIndex( index );
				}

  /** Evaluate the function at specified ContinousIndex position.
   * Subclasses must provide this method. */
  virtual typename TOutputType::Pointer
    EvaluateAtContinuousIndex( const ContinuousIndexType &cindex ) const
				{
						IndexType index;
      this->ConvertContinuousIndexToNearestIndex( cindex, index );
						return this->EvaluateAtIndex( index );
				}

  /** Evaluate the function at specified Index position.
   * Subclasses must provide this method. */
//   virtual typename TOutputType::Pointer
//     EvaluateAtIndex( const IndexType &index ) const;

  itkSetMacro( MaskImage, typename MaskImageType::Pointer );
  itkGetConstMacro( MaskImage, typename MaskImageType::Pointer );

  itkSetMacro( InsideMaskPixelValue, MaskPixelType );
  itkGetConstMacro( InsideMaskPixelValue, MaskPixelType );

  virtual void SetAnchorSeed( IndexType index )
    {
    itkDebugMacro( "setting AnchorSeed to " << index );
    if ( this->m_AnchorSeed != index )
      {
      this->m_AnchorSeed = index;
      if ( this->GetInputImage() != NULL )
        {
        this->GeneratePathDirectionImage();
        }
      this->Modified();
      }
    }
  itkGetConstMacro( AnchorSeed, IndexType );

  itkSetMacro( UseFaceConnectedness, bool );
  itkGetConstMacro( UseFaceConnectedness, bool );
  itkBooleanMacro( UseFaceConnectedness );

  itkSetMacro( UseImageSpacing, bool );
  itkGetConstMacro( UseImageSpacing, bool );
  itkBooleanMacro( UseImageSpacing );

protected:

  MinimalPathImageFunction();
  virtual ~MinimalPathImageFunction();
  void PrintSelf(std::ostream& os, Indent indent) const;

private:
  MinimalPathImageFunction(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  void GeneratePathDirectionImage();


  typename OffsetImageType::Pointer          m_PathDirectionImage;

  typename MaskImageType::Pointer            m_MaskImage;
  MaskPixelType                              m_InsideMaskPixelValue;

  IndexType                                  m_AnchorSeed;

  bool                                       m_UseFaceConnectedness;
  bool                                       m_UseImageSpacing;
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkMinimalPathImageFunction.hxx"
#endif

#endif

