/**
 * @file TrackGenerator.h
 * @brief The TrackGenerator class.
 * @date January 23, 2012
 * @author William Boyd, MIT, Course 22 (wboyd@mit.edu)
 */


#ifndef TRACKGENERATOR_H_
#define TRACKGENERATOR_H_

#ifdef __cplusplus
#define _USE_MATH_DEFINES
#ifdef SWIG
#include "Python.h"
#endif
#include "Track2D.h"
#include "Geometry.h"
#include "MOCKernel.h"
#include "segmentation_type.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <omp.h>
#include <tuple>
#endif


/**
 * @class TrackGenerator TrackGenerator.h "src/TrackGenerator.h"
 * @brief The TrackGenerator is dedicated to generating and storing Tracks
 *        which cyclically wrap across the Geometry.
 * @details The TrackGenerator creates Track and initializes boundary
 *          conditions (vacuum, reflective, or periodic) for each Track.
 */
class TrackGenerator {

protected:

  /** The number of shared memory OpenMP threads */
  int _num_threads;

  /** Number of azimuthal angles in \f$ [0, 2 \pi] \f$ */
  int _num_azim;

  /** Number of polar angles in \f$ [0, \pi] \f$ */
  int _num_polar;

  /** The requested track azimuthal spacing (cm) */
  double _azim_spacing;

  /** An integer array of the number of Tracks in a cycle for each azim angle */
  int* _tracks_per_cycle;

  /** An array of the number of cycles for each azimuthal angle */
  int* _cycles_per_azim;

  /** An array of the cycle length of each cycle for each azimuthal angle */
  double* _cycle_length;

  /** The total number of Tracks for all azimuthal angles */
  int _num_2D_tracks;

  /** An integer array of the number of Tracks starting on the x-axis for each
   *  azimuthal angle */
  int* _num_x;

  /** An integer array of the number of Tracks starting on the y-axis for each
   *  azimuthal angle */
  int* _num_y;

  /** The number of parallel groups of tracks */
  int _num_parallel_track_groups;

  /** An integer array with the Track uid separating the azimuthal, polar, and
    * periodic halfspaces */
  int* _num_tracks_by_parallel_group;

  /** An array of the azimuthal angle quadrature weights */
  FP_PRECISION* _azim_spacings;

  /** A 2D ragged array of 2D tracks (azim, track index) */
  Track2D** _tracks_2D;

  /** A 1D array of Track pointers arranged by UID */
  Track** _tracks_2D_array;

  /** Pointer to the Geometry */
  Geometry* _geometry;

  /** Boolean for whether to use Track input file (true) or not (false) */
  bool _use_input_file;

  /** Filename for the *.tracks input / output file */
  std::string _tracks_filename;

  /** OpenMP mutual exclusion locks for atomic FSR operations */
  omp_lock_t* _FSR_locks;

  /** Boolean indicating whether the Tracks have been generated (true) or not
    * (false) */
  bool _contains_2D_tracks;

  /** Boolean indicating whether 2D segments have been generated (true) or not
    * (false) */
  bool _contains_2D_segments;

  /** The quadrature set */
  Quadrature* _quadrature;

  /** The z-coord where the 2D Tracks should be created */
  double _z_coord;

  /** Boolen to indicate whether a periodic BC exists */
  bool _periodic;

  /** A 2D ragged array of 2D tracks (azim, cycle, train index) */
  Track2D**** _tracks_2D_cycle;

  /** Determines the type of track segmentation to use */
  segmentationType _segment_formation;

  /** Max optical segment length for Tracks before splitting */
  FP_PRECISION _max_optical_length;

  /** Maximum number of track segmenets in a single Track */
  int _max_num_segments;

  /** Boolean to indicate whether the segments should be dumped to file */
  bool _dump_segments;

  /** A buffer holding the computed FSR volumes */
  FP_PRECISION* _FSR_volumes;

  /** Private class methods */
  virtual void initializeTracks();
  virtual void initializeTrackReflections();
  void initializeTrackCycles();
  virtual void recalibrateTracksToOrigin();
  virtual void setTotalWeights();
  virtual void segmentize();
  virtual void setContainsSegments(bool contains_segments);
  virtual void allocateTemporarySegments();
  virtual void resetStatus();
  virtual void initializeDefaultQuadrature();
  virtual std::string getTestFilename(std::string directory);

public:

  TrackGenerator(Geometry* geometry, int num_azim, int num_polar,
                 double azim_spacing);
  virtual ~TrackGenerator();

  /* Get parameters */
  int getNumAzim();
  int getNumPolar();
  double getDesiredAzimSpacing();
  Geometry* getGeometry();
  int getNum2DTracks();
  int getNum2DSegments();
  void countSegments();
  bool getPeriodic();
  Track** get2DTracksArray();
  virtual Track** getTracksArray();
  Track2D** get2DTracks();
  double* getAzimSpacings();
  double getAzimSpacing(int azim);
  FP_PRECISION getMaxOpticalLength();
  int getMaxNumSegments();
  int getNumThreads();
  int* getTracksPerCycle();
  int* getCyclesPerAzim();
  double getCycleLength(int azim);
  int getNumX(int azim);
  int getNumY(int azim);
  void exportFSRVolumes(double* out_volumes, int num_fsrs);
  FP_PRECISION* getFSRVolumesBuffer();
  FP_PRECISION* getFSRVolumes();
  FP_PRECISION getFSRVolume(int fsr_id);
  double getZCoord();
  Quadrature* getQuadrature();
  bool getCycleDirection(int azim, int cycle, int track_index);
  FP_PRECISION retrieveMaxOpticalLength();
  omp_lock_t* getFSRLocks();
  segmentationType getSegmentFormation();
  int getNumParallelTrackGroups();
  int* getNumTracksByParallelGroupArray();
  virtual bool containsTracks();
  virtual bool containsSegments();

  /* Set parameters */
  void setNumThreads(int num_threads);
  void setNumAzim(int num_azim);
  void setNumPolar(int num_polar);
  void setDesiredAzimSpacing(double spacing);
  void setGeometry(Geometry* geometry);
  void setZCoord(double z_coord);
  void setQuadrature(Quadrature* quadrature);
  void setMaxOpticalLength(FP_PRECISION tau);
  void setMaxNumSegments(int max_num_segments);
  void setDumpSegments(bool dump_segments);

  /* Worker functions */
  void retrieve2DTrackCoords(double* coords, int num_tracks);
  void retrieve2DPeriodicCycleCoords(double* coords, int num_tracks);
  void retrieve2DReflectiveCycleCoords(double* coords, int num_tracks);
  void retrieve2DSegmentCoords(double* coords, int num_segments);
  void generateFSRCentroids(FP_PRECISION* FSR_volumes);
  void generateTracks();
  void splitSegments(FP_PRECISION max_optical_length);
  double leastCommonMultiple(double a, double b);
  void dumpSegmentsToFile();
  bool readSegmentsFromFile();
  void initializeTrackFileDirectory();
  virtual void initializeTrackPeriodicIndices();
  virtual void initializeTracksArray();
  virtual void checkBoundaryConditions();
  virtual void initializeTrackCycleIds();
};



#endif /* TRACKGENERATOR_H_ */
