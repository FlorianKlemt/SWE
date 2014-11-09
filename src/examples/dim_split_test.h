/**
 * @file
 * This file is an CxxTest file to test different swe1d scenarios
 **/
#include <cassert>
#include <cstdlib>
#include <string>
#include <iostream>


#include <cxxtest/TestSuite.h>
#include <cstdlib>
#include "../blocks/SWE_Block.cpp"
#include <WavePropagation.cpp>
#include "scenarios/SWE_simple_scenarios.hh"

/**
 * Class to test SWE against SWE1D
 */
class dim_split_test : public CxxTest::TestSuite
{
	float tol=0.001f;

public:
	/*
	 * Testing SWE against SWE1D
	 */
	void testSWE_dimsplit(void)
	{
		TS_TRACE("Starting test: Dimensional splitting");

		//! number of grid cells in x- and y-direction.
		int l_nX=100;
		int l_nY=100;

		// create a simple artificial scenario
		SWE_RadialDamBreakScenario l_scenario;

		//! size of a single cell in x- and y-direction
		float l_dX, l_dY;

		  // compute the size of a single cell
		l_dX = (l_scenario.getBoundaryPos(BND_RIGHT) - l_scenario.getBoundaryPos(BND_LEFT) )/l_nX;
		l_dY = (l_scenario.getBoundaryPos(BND_TOP) - l_scenario.getBoundaryPos(BND_BOTTOM) )/l_nY;

		  // create a single wave propagation block
		SWE_DimensionalSplitting SWE(l_nX,l_nY,l_dX,l_dY);
		SWE_DimensionalSplitting SWE1D(l_nX,l_nY,l_dX,l_dY);


		//! origin of the simulation domain in x- and y-direction
		float l_originX, l_originY;

		// get the origin from the scenario
		l_originX = l_scenario.getBoundaryPos(BND_LEFT);
		l_originY = l_scenario.getBoundaryPos(BND_BOTTOM);


		// initialize the wave propagation block
		SWE.initScenario(l_originX, l_originY, l_scenario);
		SWE1D.initScenario(l_originX, l_originY, l_scenario);

		//Calculating values with SWE
		SWE.computeNumericalFluxes();
		//! maximum allowed time step width.
		float l_maxTimeStepWidth = SWE.getMaxTimestep();

		// update the cell values
		SWE.updateUnknowns(l_maxTimeStepWidth);

		//Calculating Values with SWE1D

		//Float2D h,b,hu,hv;
		Float2D h(SWE1D.getNx()+2, SWE1D.getNy()+2);
		Float2D hu(SWE1D.getNx()+2, SWE1D.getNy()+2);
		Float2D hv(SWE1D.getNx()+2, SWE1D.getNy()+2);
		Float2D b(SWE1D.getNx()+2, SWE1D.getNy()+2);


		//Initializing values
		for(int i=1; i<=SWE1D.getNx(); i++)
		    for(int j=1; j<=SWE1D.getNy(); j++) {

		      h[i][j] =  SWE1D.getWaterHeight()[i][j];
		      hu[i][j] = SWE1D.getDischarge_hu()[i][j];
		      hv[i][j] = SWE1D.getDischarge_hv[i][j];
		    };

		  // initialize bathymetry
		  for(int i=0; i<=SWE1D.getNx()+1; i++) {
		    for(int j=0; j<=SWE1D.getNy()+1; j++) {
		      b[i][j] = SWE1D.getBathymetry()[i][j];
		    }


		//Loop in x direction
		for(int j=1;j<SWE1D.getNy();j++)
		{
			for(int i=1;i<SWE1D.getNx();i++)
			{

				T *tmp_h = new T[SWE1D.getNx()+2];
				T *tmp_hu = new T[SWE1D.getNx()+2];
				T *tmp_b = new T[SWE1D.getNx()+2];

				tmp_h[i] = h[i][j];
				tmp_hu[i] = hu[i][j];
				tmp_b[i] = b[i][j];

				printf("%f\n",tmp_h[i]);
				printf("%f\n",tmp_hu[i]);
				printf("%f\n",tmp_b[i]);

				WavePropagation wavePropagation(tmp_h, tmp_hu, tmp_b, l_nX, l_dX);

				// Compute numerical flux on each edge
				T maxTimeStep = wavePropagation.computeNumericalFluxes();


				//Save calculated Values
				h[i][j] = tmp_h[i];
				hu[i][j] = tmp_hu[i];
				b[i][j] = tmp_b[i];
			}
		}


		//loop in y direction
		for(int i=0;SWE1D.getNx()+2;i++)
		{
			for(int j=0;SWE1D.getNy()+2;j++)
			{
				T *tmp_h = new T[SWE1D.getNx()+2];
				T *tmp_hv = new T[SWE1D.getNx()+2];
				T *tmp_b = new T[SWE1D.getNx()+2];

				tmp_h[j] = h[i][j];
				tmp_hv[j] = SWE1D.getDischarge_hv()[i][j];
				tmp_b[j] = b[i][j];

				WavePropagation wavePropagation(tmp_h, tmp_hv,tmp_b, l_nY, l_dY);

				// Compute numerical flux on each edge
				T maxTimeStep = wavePropagation.computeNumericalFluxes();

				//Save calculated Values
				h[i][j] = tmp_h[i];
				hv[i][j] = tmp_hv[i];
				b[i][j] = tmp_b[i];

			}
		}


		for(int i=0;i<SWE.getNx()+2;i++)
		{
			for(int j=0;j<SWE.getNy()+2;j++)
			{
				TS_ASSERT_DELTA(h[i][j], SWE.getWaterHeight()[i][j] ,tol);
				TS_ASSERT_DELTA(hu[i][j], SWE.getDischarge_hu()[i][j],tol);
				TS_ASSERT_DELTA(hv[i][j], SWE.getDischarge_hv()[i][j],tol);
				TS_ASSERT_DELTA(b[i][j], SWE.getBathymetry()[i][j],tol);
			}
		}



		TS_TRACE("Finishing test: Dimensional splitting");
	}
};
