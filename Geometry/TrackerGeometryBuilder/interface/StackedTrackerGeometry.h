/*! \class   StackedTrackerGeometry
 *  \brief   TrackerGeometry-derived class for Pt modules
 *  \details
 *
 *  \author Andrew W. Rose
 *  \author Nicola Pozzobon
 *  \author Ivan Reid
 *  \date   2008
 *
 */

#ifndef STACKED_TRACKER_GEOMETRY_H
#define STACKED_TRACKER_GEOMETRY_H

#include <typeinfo>

#include "DataFormats/SiPixelDetId/interface/StackedTrackerDetId.h"
#include "DataFormats/GeometrySurface/interface/Plane.h"
#include "SimDataFormats/SLHC/interface/L1TkStub.h" //yes this needs to be a DataFormat eventually
#include "DataFormats/L1TrackTrigger/interface/TTStub.h"
#include "Geometry/CommonDetUnit/interface/GeomDetUnit.h"
#include "Geometry/CommonTopologies/interface/Topology.h"
#include "Geometry/TrackerGeometryBuilder/interface/PixelGeomDetUnit.h"
#include "Geometry/CommonTopologies/interface/PixelTopology.h"

#include <vector>
#include <ext/hash_map>

class StackedTrackerDetUnit;
class GeomDet;
class TrackerGeometry;

class StackedTrackerGeometry
{
  public:
    typedef std::vector< StackedTrackerDetUnit* >                       StackContainer;
    typedef std::vector< StackedTrackerDetUnit* >::const_iterator       StackContainerIterator;
    typedef std::vector< StackedTrackerDetId >                          StackIdContainer;
    typedef std::vector< StackedTrackerDetId >::const_iterator          StackIdContainerIterator;
    typedef __gnu_cxx::hash_map< unsigned int, StackedTrackerDetUnit* > mapIdToStack;

    /// Constructors
    StackedTrackerGeometry( const TrackerGeometry *i );
    StackedTrackerGeometry( const TrackerGeometry *i, const int partitionsPerRoc, const unsigned CBC3_Stubs );
    virtual ~StackedTrackerGeometry();  

    /// Methods for data members
    const StackContainer&   stacks() const   { return theStacks; }
    const StackIdContainer& stackIds() const { return theStackIds; } 
    void  addStack( StackedTrackerDetUnit *aStack );
    const StackedTrackerDetUnit* idToStack( StackedTrackerDetId anId ) const;

    /// Analogous to the methods in TrackerGeomety except that you pass it a stack id and an identifier to a stack member
    const GeomDetUnit* idToDetUnit( StackedTrackerDetId anId, unsigned int stackMemberIdentifier ) const;
    const GeomDet*     idToDet( StackedTrackerDetId anId, unsigned int stackMemberIdentifier )     const;

    /// Association Detector/Module/Stack
    DetId findPairedDetector( DetId anId ) const;
    DetId findStackFromDetector( DetId anId ) const;

    /// Specific module properties
    const bool isPSModule( StackedTrackerDetId anId ) const;

    /// Dedicated to CBC3 emulation
    /// Everything is in half-strip units
    const int getDetUnitWindow( StackedTrackerDetId anId ) const;
    const int getASICOffset( StackedTrackerDetId anId, int asicNumber, int partitionNumber ) const;
    int       getPartitionsPerRoc() const { return theNumPartitions; }
    unsigned  getCBC3MaxStubs() const     { return theMaxStubs; }

    /// Helper functions
    Plane::PlanePointer meanPlane( StackedTrackerDetId anId ) const;

    /// Stub functions
    /// OLD STYLE
    template< typename T > double       findRoughPt( double aMagneticFieldStrength, const L1TkStub<T> *stub) const;
    template< typename T > GlobalPoint  findGlobalPosition( const L1TkStub<T> *stub ) const;
    template< typename T > GlobalVector findGlobalDirection( const L1TkStub<T> *stub ) const;

    /// Stub functions
    /// NEW style
    template< typename T > double       findRoughPt( double aMagneticFieldStrength, const TTStub<T> *stub) const;
    template< typename T > GlobalPoint  findGlobalPosition( const TTStub<T> *stub ) const;
    template< typename T > GlobalVector findGlobalDirection( const TTStub<T> *stub ) const;
 
    /// Cluster functions
    /// OLD STYLE
    template< typename T > LocalPoint       findHitLocalPosition( const L1TkCluster<T> *cluster , unsigned int hitIdx ) const;
    template< typename T > GlobalPoint      findHitGlobalPosition( const L1TkCluster<T> *cluster,  unsigned int hitIdx ) const;
    template< typename T > LocalPoint       findAverageLocalPosition(  const L1TkCluster<T> *cluster  ) const;
    template< typename T > GlobalPoint      findAverageGlobalPosition( const L1TkCluster<T> *cluster ) const;
    template< typename T > void checkSimTrack( L1TkCluster<T> *cluster,
                                               edm::Handle<edm::DetSetVector<PixelDigiSimLink> > thePixelDigiSimLinkHandle,
                                               edm::Handle<edm::SimTrackContainer> simTrackHandle ) const;

    /// Cluster functions
    /// NEW STYLE
    template< typename T > LocalPoint       findHitLocalPosition( const TTCluster<T> *cluster , unsigned int hitIdx ) const;
    template< typename T > GlobalPoint      findHitGlobalPosition( const TTCluster<T> *cluster,  unsigned int hitIdx ) const;
    template< typename T > LocalPoint       findAverageLocalPosition(  const TTCluster<T> *cluster  ) const;
    template< typename T > GlobalPoint      findAverageGlobalPosition( const TTCluster<T> *cluster ) const;

  private:

    /// Data members
    const TrackerGeometry* theTracker;
    StackContainer         theStacks;
    StackIdContainer       theStackIds;
    mapIdToStack           theMap;

    /// Associative containers
    std::map< DetId, DetId > mapDetectorsToPartner;
    std::map< DetId, DetId > mapDetectorsToStack;

    /// CBC3 dedicated stuff
    int      theNumPartitions; // Partitions per ROC for offset calculations as per CBC3 chip
    unsigned theMaxStubs; // Maximum number of stubs per CBC3 chip; will need rethink when PS chip designed

};

/// Bunch of specializations

/// Cluster stuff
/// OLD STYLE
template<>
LocalPoint StackedTrackerGeometry::findHitLocalPosition( const L1TkCluster< edm::Ref< edm::PSimHitContainer > > *cluster, unsigned int hitIdx ) const;
template<>
GlobalPoint StackedTrackerGeometry::findHitGlobalPosition( const L1TkCluster< edm::Ref< edm::PSimHitContainer > > *cluster, unsigned int hitIdx ) const;
template<>
LocalPoint StackedTrackerGeometry::findHitLocalPosition( const L1TkCluster< edm::Ref< edm::PSimHitContainer > > *cluster, unsigned int hitIdx ) const;
template<>
void StackedTrackerGeometry::checkSimTrack( L1TkCluster< edm::Ref< edm::PSimHitContainer > > *cluster,
                                            edm::Handle<edm::DetSetVector<PixelDigiSimLink> >  thePixelDigiSimLinkHandle,
                                            edm::Handle<edm::SimTrackContainer> simTrackHandle ) const;

/// Fit Stub as in Builder
/// To be used for out-of-Builder Stubs
/// OLD STYLE
template< typename T >
double StackedTrackerGeometry::findRoughPt( double aMagneticFieldStrength, const L1TkStub<T> *stub) const
{
  /// Get the magnetic field
  /// Usually it is done like the following three lines
  //iSetup.get<IdealMagneticFieldRecord>().get(magnet);
  //magnet_ = magnet.product();
  //mMagneticFieldStrength = magnet_->inTesla(GlobalPoint(0,0,0)).z();
  /// Calculate factor for rough Pt estimate
  /// B rounded to 4.0 or 3.8
  /// This is B * C / 2 * appropriate power of 10
  /// So it's B * 0.0015
  double mPtFactor = (floor(aMagneticFieldStrength*10.0 + 0.5))/10.0*0.0015;

  /// Get average position of Clusters composing the Stub
  GlobalPoint innerHitPosition = findAverageGlobalPosition( stub->getClusterPtr(0).get() );
  GlobalPoint outerHitPosition = findAverageGlobalPosition( stub->getClusterPtr(1).get() );

  /// Get useful quantities
  double outerPointRadius = outerHitPosition.perp();
  double innerPointRadius = innerHitPosition.perp();
  double outerPointPhi = outerHitPosition.phi();
  double innerPointPhi = innerHitPosition.phi();
  double deltaRadius = outerPointRadius - innerPointRadius;

  /// Here a switch on Barrel/Endcap is introduced
  StackedTrackerDetId tempDetId(stub->getDetId());
  if (tempDetId.isBarrel())
  {
    /// Calculate angular displacement from hit phi locations
    /// and renormalize it, if needed
    double deltaPhi = outerPointPhi - innerPointPhi;
    if (deltaPhi < 0)
      deltaPhi = -deltaPhi;
    if (deltaPhi > M_PI)
      deltaPhi = 2*M_PI - deltaPhi;

    /// Return the rough Pt
    return ( deltaRadius * mPtFactor / deltaPhi );
  }
  else if (tempDetId.isEndcap())
  {
    /// Test approximated formula for Endcap stubs
    /// Check always to be consistent with HitMatchingAlgorithm_window2012.h
    double roughPt = innerPointRadius * innerPointRadius * mPtFactor / fabs(findAverageLocalPosition( stub->getClusterPtr(0).get() ).x()) ;
    roughPt +=       outerPointRadius * outerPointRadius * mPtFactor / fabs(findAverageLocalPosition( stub->getClusterPtr(1).get() ).x()) ;
    roughPt = roughPt / 2.;

    /// Return the rough Pt
    return roughPt;
  }

  /// Default
  /// Should never get here
  std::cerr << "W A R N I N G! L1TkStub::findRoughPt() \t we should never get here" << std::endl;
  return 99999.9;
}

template< typename T >
GlobalPoint StackedTrackerGeometry::findGlobalPosition( const L1TkStub< T > *stub ) const
{
  /// Fast version: only inner cluster matters
  return findAverageGlobalPosition( stub->getClusterPtr(0).get() );
}

template< typename T >
GlobalVector StackedTrackerGeometry::findGlobalDirection( const L1TkStub< T > *stub ) const
{
  /// Get average position of Clusters composing the Stub
  GlobalPoint innerHitPosition = findAverageGlobalPosition( stub->getClusterPtr(0).get() );
  GlobalPoint outerHitPosition = findAverageGlobalPosition( stub->getClusterPtr(1).get() );

  /// Calculate the direction
  GlobalVector directionVector( outerHitPosition.x()-innerHitPosition.x(),
                                outerHitPosition.y()-innerHitPosition.y(),
                                outerHitPosition.z()-innerHitPosition.z() );

  return directionVector;
}

/// Get hit local position
/// Default template for PixelDigis
  template< typename T >
  LocalPoint StackedTrackerGeometry::findHitLocalPosition( const L1TkCluster< T > *cluster, unsigned int hitIdx ) const
{
  /// Add 0.5 to get the center of the pixel
  const GeomDetUnit* geomDetUnit = idToDetUnit( cluster->getDetId(), cluster->getStackMember() );
  T hit = cluster->getHits().at(hitIdx); 
  MeasurementPoint mp( hit->row() + 0.5, hit->column() + 0.5 );
  return geomDetUnit->topology().localPosition( mp );
}

/// Get hit global position
/// Default template for PixelDigis
  template< typename T >
  GlobalPoint StackedTrackerGeometry::findHitGlobalPosition( const L1TkCluster< T > *cluster, unsigned int hitIdx ) const
{
  /// Add 0.5 to get the center of the pixel
  const GeomDetUnit* geomDetUnit = idToDetUnit( cluster->getDetId(), cluster->getStackMember() );
  T hit = cluster->getHits().at(hitIdx); 
  MeasurementPoint mp( hit->row() + 0.5, hit->column() + 0.5 );
  return geomDetUnit->surface().toGlobal( geomDetUnit->topology().localPosition( mp ) );
}

/// Unweighted average local cluster position
template< typename T >
LocalPoint StackedTrackerGeometry::findAverageLocalPosition( const L1TkCluster< T > *cluster ) const
{
  double averageX = 0.0;
  double averageY = 0.0;

  /// Loop over the hits and calculate the average coordinates
  std::vector<T> hits=cluster->getHits();

  if ( hits.size() != 0 )
  {
    for ( unsigned int i = 0; i < hits.size(); i++ )
    {
      LocalPoint thisHitPosition = findHitLocalPosition( cluster, i );
      averageX += thisHitPosition.x();
      averageY += thisHitPosition.y();
    }
    averageX /= hits.size();
    averageY /= hits.size();
  }
  return LocalPoint( averageX, averageY );
}

/// Unweighted average cluster position
template< typename T >
GlobalPoint StackedTrackerGeometry::findAverageGlobalPosition( const L1TkCluster< T > *cluster ) const
{
  double averageX = 0.0;
  double averageY = 0.0;
  double averageZ = 0.0;

  std::vector<T> hits=cluster->getHits();

  /// Loop over the hits and calculate the average coordinates
  if ( hits.size() != 0 )
  {
    for ( unsigned int i = 0; i < hits.size(); i++ )
    {
      GlobalPoint thisHitPosition = findHitGlobalPosition( cluster, i );
      averageX += thisHitPosition.x();
      averageY += thisHitPosition.y();
      averageZ += thisHitPosition.z();
    }
    averageX /= hits.size();
    averageY /= hits.size();
    averageZ /= hits.size();
  }
  return GlobalPoint( averageX, averageY, averageZ );
}

template< typename T >
void StackedTrackerGeometry::checkSimTrack( L1TkCluster< T > *cluster,
                                            edm::Handle<edm::DetSetVector<PixelDigiSimLink> >  thePixelDigiSimLinkHandle,
                                            edm::Handle<edm::SimTrackContainer>   simTrackHandle ) const
{
  /// Discrimination between MC and data in Builder!

  /// Get the PixelDigiSimLink
  const DetId detId = idToDet( cluster->getDetId(), cluster->getStackMember() )->geographicalId();
  edm::DetSet<PixelDigiSimLink> thisDigiSimLink = (*(thePixelDigiSimLinkHandle) )[detId.rawId()];
  edm::DetSet<PixelDigiSimLink>::const_iterator iterSimLink;

  std::vector<T> hits=cluster->getHits(); 
  /// Loop over all the hits composing the L1TkCluster
  for ( unsigned int i = 0; i < hits.size(); i++ )
  {
    /// Loop over PixelDigiSimLink
    for ( iterSimLink = thisDigiSimLink.data.begin();
          iterSimLink != thisDigiSimLink.data.end();
          iterSimLink++ )
    {
      /// Threshold (redundant, already applied within L1TkClusterBuilder)
      //if ( theHit.adc() <= 30 ) continue;
      /// Find the link and, if there's not, skip
      if ( (int)iterSimLink->channel() != hits.at(i)->channel() ) continue;

      /// Get SimTrack Id and type
      unsigned int curSimTrkId = iterSimLink->SimTrackId();
      /// This version of the collection of the SimTrack ID and PDG
      /// may not be fast and optimal, but is safer since the
      /// SimTrack ID is shifted by 1 wrt the index in the vector,
      /// and this may not be so true on a general basis...
      bool foundSimTrack = false;
      for ( unsigned int j = 0; j < simTrackHandle->size() && !foundSimTrack; j++ )
      {
        if ( simTrackHandle->at(j).trackId() == curSimTrkId )
        {
          foundSimTrack = true;
          edm::Ptr< SimTrack > testSimTrack( simTrackHandle, j );
          cluster->addSimTrack( testSimTrack );
          cluster->addEventId( iterSimLink->eventId().rawId() );
        }
      }

      if ( !foundSimTrack )
      {
        edm::Ptr< SimTrack > testSimTrack;
        cluster->addSimTrack( testSimTrack );
        cluster->addEventId( iterSimLink->eventId().rawId() );
      }
    }
  } /// End of Loop over all the hits composing the L1TkCluster
}

/// Stub Pt
/// NEW STYLE
template< typename T >
double StackedTrackerGeometry::findRoughPt( double aMagneticFieldStrength, const TTStub<T> *stub) const
{
  /// Get the magnetic field
  /// Usually it is done like the following three lines
  //iSetup.get<IdealMagneticFieldRecord>().get(magnet);
  //magnet_ = magnet.product();
  //mMagneticFieldStrength = magnet_->inTesla(GlobalPoint(0,0,0)).z();
  /// Calculate factor for rough Pt estimate
  /// B rounded to 4.0 or 3.8
  /// This is B * C / 2 * appropriate power of 10
  /// So it's B * 0.0015
  double mPtFactor = (floor(aMagneticFieldStrength*10.0 + 0.5))/10.0*0.0015;

  /// Get average position of Clusters composing the Stub
  GlobalPoint innerHitPosition = findAverageGlobalPosition( stub->getClusterRef(0).get() );
  GlobalPoint outerHitPosition = findAverageGlobalPosition( stub->getClusterRef(1).get() );

  /// Get useful quantities
  double outerPointRadius = outerHitPosition.perp();
  double innerPointRadius = innerHitPosition.perp();
  double outerPointPhi = outerHitPosition.phi();
  double innerPointPhi = innerHitPosition.phi();
  double deltaRadius = outerPointRadius - innerPointRadius;

  /// Here a switch on Barrel/Endcap is introduced
  StackedTrackerDetId tempDetId(stub->getDetId());
  if (tempDetId.isBarrel())
  {
    /// Calculate angular displacement from hit phi locations
    /// and renormalize it, if needed
    double deltaPhi = outerPointPhi - innerPointPhi;
    if (deltaPhi < 0)
      deltaPhi = -deltaPhi;
    if (deltaPhi > M_PI)
      deltaPhi = 2*M_PI - deltaPhi;

    /// Return the rough Pt
    return ( deltaRadius * mPtFactor / deltaPhi );
  }
  else if (tempDetId.isEndcap())
  {
    /// Test approximated formula for Endcap stubs
    /// Check always to be consistent with HitMatchingAlgorithm_window2012.h
    double roughPt = innerPointRadius * innerPointRadius * mPtFactor / fabs(findAverageLocalPosition( stub->getClusterRef(0).get() ).x()) ;
    roughPt +=       outerPointRadius * outerPointRadius * mPtFactor / fabs(findAverageLocalPosition( stub->getClusterRef(1).get() ).x()) ;
    roughPt = roughPt / 2.;

    /// Return the rough Pt
    return roughPt;
  }

  /// Default
  /// Should never get here
  std::cerr << "W A R N I N G! TTStub::findRoughPt() \t we should never get here" << std::endl;
  return 99999.9;
}

template< typename T >
GlobalPoint StackedTrackerGeometry::findGlobalPosition( const TTStub< T > *stub ) const
{
  /// Fast version: only inner cluster matters
  return findAverageGlobalPosition( stub->getClusterRef(0).get() );
}

template< typename T >
GlobalVector StackedTrackerGeometry::findGlobalDirection( const TTStub< T > *stub ) const
{
  /// Get average position of Clusters composing the Stub
  GlobalPoint innerHitPosition = findAverageGlobalPosition( stub->getClusterRef(0).get() );
  GlobalPoint outerHitPosition = findAverageGlobalPosition( stub->getClusterRef(1).get() );

  /// Calculate the direction
  GlobalVector directionVector( outerHitPosition.x()-innerHitPosition.x(),
                                outerHitPosition.y()-innerHitPosition.y(),
                                outerHitPosition.z()-innerHitPosition.z() );

  return directionVector;
}

/// Get hit local position
/// Default template for PixelDigis
template< typename T >
LocalPoint StackedTrackerGeometry::findHitLocalPosition( const TTCluster< T > *cluster, unsigned int hitIdx ) const
{
  /// Add 0.5 to get the center of the pixel
  const GeomDetUnit* geomDetUnit = idToDetUnit( cluster->getDetId(), cluster->getStackMember() );
  int row = 0;
  int col = 0;

  if ( cluster->getRows().size() == 0 || cluster->getCols().size() == 0 )
  {
    T hit = cluster->getHits().at(hitIdx);
    row = hit->row();
    col = hit->column();
  }
  else
  {
    row = cluster->getRows().at(hitIdx);
    col = cluster->getCols().at(hitIdx);
  } 
  MeasurementPoint mp( row + 0.5, col + 0.5 );
  return geomDetUnit->topology().localPosition( mp );
}

/// Get hit global position
/// Default template for PixelDigis
template< typename T >
GlobalPoint StackedTrackerGeometry::findHitGlobalPosition( const TTCluster< T > *cluster, unsigned int hitIdx ) const
{
  /// Add 0.5 to get the center of the pixel
  const GeomDetUnit* geomDetUnit = idToDetUnit( cluster->getDetId(), cluster->getStackMember() );
  int row = 0;
  int col = 0;

  if ( cluster->getRows().size() == 0 || cluster->getCols().size() == 0 )
  {
    T hit = cluster->getHits().at(hitIdx);
    row = hit->row();
    col = hit->column();
  }
  else
  {
    row = cluster->getRows().at(hitIdx);
    col = cluster->getCols().at(hitIdx);
  }
  MeasurementPoint mp( row + 0.5, col + 0.5 );
  return geomDetUnit->surface().toGlobal( geomDetUnit->topology().localPosition( mp ) );
}

/// Unweighted average local cluster position
template< typename T >
LocalPoint StackedTrackerGeometry::findAverageLocalPosition( const TTCluster< T > *cluster ) const
{
  double averageX = 0.0;
  double averageY = 0.0;

  /// Loop over the hits and calculate the average coordinates
  std::vector<T> hits=cluster->getHits();

  if ( hits.size() != 0 )
  {
    for ( unsigned int i = 0; i < hits.size(); i++ )
    {
      LocalPoint thisHitPosition = findHitLocalPosition( cluster, i );
      averageX += thisHitPosition.x();
      averageY += thisHitPosition.y();
    }
    averageX /= hits.size();
    averageY /= hits.size();
  }
  return LocalPoint( averageX, averageY );
}

/// Unweighted average cluster position
template< typename T >
GlobalPoint StackedTrackerGeometry::findAverageGlobalPosition( const TTCluster< T > *cluster ) const
{
  double averageX = 0.0;
  double averageY = 0.0;
  double averageZ = 0.0;

  std::vector<T> hits=cluster->getHits();

  /// Loop over the hits and calculate the average coordinates
  if ( hits.size() != 0 )
  {
    for ( unsigned int i = 0; i < hits.size(); i++ )
    {
      GlobalPoint thisHitPosition = findHitGlobalPosition( cluster, i );
      averageX += thisHitPosition.x();
      averageY += thisHitPosition.y();
      averageZ += thisHitPosition.z();
    }
    averageX /= hits.size();
    averageY /= hits.size();
    averageZ /= hits.size();
  }
  return GlobalPoint( averageX, averageY, averageZ );
}

#endif

