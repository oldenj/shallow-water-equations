#ifndef SWE_DIMENSIONALSPLITTINGCHARM_HH
#define SWE_DIMENSIONALSPLITTINGCHARM_HH

#include "SWE_DimensionalSplittingCharm.decl.h"

#include <unistd.h>
#include <limits.h>
#include <ctime>
#include <time.h>
#include "blocks/SWE_Block.hh"
#ifdef ASAGI
#include "scenarios/SWE_AsagiScenario.hh"
#else
#include "scenarios/SWE_simple_scenarios.hh"
#endif
#include "examples/swe_charm.decl.h"
#include "types/Boundary.hh"
#include "writer/NetCdfWriter.hh"
#include "tools/Float2DNative.hh"
#include "solvers/Hybrid.hpp"

extern CProxy_swe_charm mainProxy;
extern int blockCountX;
extern int blockCountY;
extern float simulationDuration;
extern int checkpointCount;

class SWE_DimensionalSplittingCharm : public CBase_SWE_DimensionalSplittingCharm, public SWE_Block<Float2DNative>  {

	SWE_DimensionalSplittingCharm_SDAG_CODE

	public:
		// Charm++ specific constructor needed for object migration
		SWE_DimensionalSplittingCharm(CkMigrateMessage *msg);
		SWE_DimensionalSplittingCharm(int cellCountHorizontal, int cellCountVertical, float cellSizeHorizontal, float cellSizeVertical,
						float originX, float originY, int posX, int posY, BoundaryType boundaries[],
						std::string outputFileName, std::string bathymetryFileName = "", std::string displacementFileName = "");
		~SWE_DimensionalSplittingCharm();

		// Charm++ entry methods
		void reduceWaveSpeed(float maxWaveSpeed);

	private:
		void writeTimestep();
		void sendCopyLayers(bool sendBathymetry = false);
		void processCopyLayer(copyLayer *msg);
		void computeNumericalFluxes();
		void updateUnknowns(float dt);
		// Interface implementation
		void setGhostLayer();

		solver::Hybrid<float> solver;
		float *checkpointInstantOfTime;
		NetCdfWriter *writer;
		float currentSimulationTime;
		int currentCheckpoint;

		// net updates per cell
		Float2DNative hNetUpdatesLeft;
		Float2DNative hNetUpdatesRight;

		Float2DNative huNetUpdatesLeft;
		Float2DNative huNetUpdatesRight;

		Float2DNative hNetUpdatesBelow;
		Float2DNative hNetUpdatesAbove;

		Float2DNative hvNetUpdatesBelow;
		Float2DNative hvNetUpdatesAbove;

		// Interfaces to neighbouring block copy layers, indexed by Boundary
		int neighbourIndex[4];

		// timer
		std::clock_t computeClock;

		struct timespec startTime;
		struct timespec endTime;

		struct timespec startTimeCompute;
		struct timespec endTimeCompute;

		float computeTime;
		float computeTimeWall;
		float wallTime;
};

class copyLayer : public CMessage_copyLayer {
	public:
		Boundary boundary;
		bool containsBathymetry;
		float *b;
		float *h;
		float *hu;
		float *hv;
};

#endif // SWE_DIMENSIONALSPLITTINGCHARM_HH
