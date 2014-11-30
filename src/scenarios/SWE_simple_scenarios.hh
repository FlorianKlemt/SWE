/**
 * @file
 * This file is part of SWE.
 *
 * @author Michael Bader, Kaveh Rahnema, Tobias Schnabel
 * @author Sebastian Rettenberger (rettenbs AT in.tum.de, http://www5.in.tum.de/wiki/index.php/Sebastian_Rettenberger,_M.Sc.)
 *
 * @section LICENSE
 *
 * SWE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SWE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SWE.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * @section DESCRIPTION
 *
 * TODO
 */

#ifndef __SWE_SIMPLE_SCENARIOS_H
#define __SWE_SIMPLE_SCENARIOS_H

#include <cmath>
#include <regex>

#include "SWE_Scenario.hh"


/*
 * Artificial Tsunami without NetCDF
 */
class SWE_ArtificialTsunamiScenario : public SWE_Scenario {

  public:

    float getBathymetry(float x, float y) {
    	return -100;
    };

    float getWaterHeight(float x, float y) {
    	if(x>4500 && x<5500 && y>4500 && y<5500){
    		float x_relative=x-5000;
    		float y_relative=y-5000;
    		return 100+(5*sin(((x_relative/500)+1)*M_PI)*(-pow((y_relative/500),2)+1));
    	}else{
    		return 100;
    	}
    };

	virtual float endSimulation() {
		return (float) 100;
	};

    virtual BoundaryType getBoundaryType(BoundaryEdge edge) {
    	return OUTFLOW;
    };

    /** Get the boundary positions
     *
     * @param i_edge which edge
     * @return value in the corresponding dimension
     */
    float getBoundaryPos(BoundaryEdge i_edge) {
       if ( i_edge == BND_LEFT )
         return (float)0;
       else if ( i_edge == BND_RIGHT)
         return (float)10000;
       else if ( i_edge == BND_BOTTOM )
         return (float)0;
       else
         return (float)10000;
    };
};




/*
 * Tsunami Scenario with NetDCF
 * (Linker Failure!!)
 */
class TsunamiScenario : public SWE_Scenario {
  private:
	int simTime;
	std::string boundaryCond;

  public:

    float getBathymetry(float x, float y) {
    	float xpositions[],ypositions[];
    	float bathymetry[][],disp[][];
    	/*parse Bathymetry
    	  suppose xvalues are in xpositions array,
    	   yvalues in ypositions array and bathymetry data
    	   in 2-dimnesional array bathymetry
    	   suppose displacement data is in 2-dimensional array disp
		*/
    	float closestx,closesty;
    	//get closest x-value from the dataset to the required x value
    	float low_x,high_x,low_y,high_y;
    	float i;
    	for(i=0;i<sizeof(xpositions);i++){
    		if(x<xpositions[i]){
    			low_x=xpositions[i];
    			continue;
    		}else{
    			high_x=xpositions[i];
    			break;
    		}
    	}
    	closestx=min(abs(high_x-x),abs(low_x-x));
    	if(abs(low_x-x)<abs(high_x-x)){
    		i--;
    	}
    	//i ist jetzt arrayindex des nächsten x-Wertes

    	//get closest y-value from the dataset to the required y value
        float j;
    	for(j=0;j<sizeof(ypositions);j++){
    	    if(y<ypositions[j]){
    			low_y=ypositions[j];
    	    	continue;
    	    }else{
    	    	high_y=ypositions[j];
    	    	break;
    	    }
    	 }
    	 closesty=min(abs(high_y-y),abs(low_y-y));
    	 if(abs(low_y-y)<abs(high_y-y)){
    	     j--;
    	 }
    	 //j ist jetzt arrayindex des nächsten y-Wertes

    	 //bathymetry nach erdbeben = bathymetry vor erdbeben + disp
    	float bath= bathymetry[i][j]+disp[i][j];
    	//regulate bathymetry to range -20...20
    	if(bath>20){
    		return 20;
    	}else if(bath<-20){
    		return -20;
    	}else{
    		return bath;
    	}
    };

    float getWaterHeight(float x, float y) {
    	float xpositions[],ypositions[];
    	float bathymetry[][];
    	//parse WaterHeight
    	//similar to structure in getBathymetry
		float closestx, closesty;
		float low_x, high_x, low_y, high_y;
		float i;
		for (i = 0; i < sizeof(xpositions); i++) {
			if (x < xpositions[i]) {
				low_x = xpositions[i];
				continue;
			} else {
				high_x = xpositions[i];
				break;
			}
		}
		closestx = min(abs(high_x - x), abs(low_x - x));
		if (abs(low_x - x) < abs(high_x - x)) {
			i--;
		}
		float j;
		for (j = 0; j < sizeof(ypositions); j++) {
			if (y < ypositions[j]) {
				low_y = ypositions[j];
				continue;
			} else {
				high_y = ypositions[j];
				break;
			}
		}
		closesty = min(abs(high_y - y), abs(low_y - y));
		if (abs(low_y - y) < abs(high_y - y)) {
			j--;
		}

		//water height =-min(bathymetry vor erdbeben,0)
		return (bathymetry[i][j]>0)?bathymetry[i][j]:0;
    };

	virtual float endSimulation() {
		return simTime;
	};

    virtual BoundaryType getBoundaryType(BoundaryEdge edge) {
    	return boundaryCond;
    };

    /** Get the boundary positions
     *
     * @param i_edge which edge
     * @return value in the corresponding dimension
     */
    float getBoundaryPos(BoundaryEdge i_edge) {
       if ( i_edge == BND_LEFT )
         return (float)0;
       else if ( i_edge == BND_RIGHT)
         return (float)10000;
       else if ( i_edge == BND_BOTTOM )
         return (float)0;
       else
         return (float)10000;
    };
};





/**
 * Scenario "Radial Dam Break":
 * elevated water in the center of the domain
 */
class SWE_RadialDamBreakScenario : public SWE_Scenario {

  public:

    float getBathymetry(float x, float y) {
    	float a = 5.0f;
    	float b = 10.0f;
    	float x0 = 700.0f;
    	float y0 = 650.0;
    	float r = 250.0f;

    	if(((x-x0)*(x-x0))/(a*a)+((y-y0)*(y-y0))/(b*b)<r)
    	{
    		return 5.0f;
    	}
    	return -5.0f;
    };

    float getWaterHeight(float x, float y) { 
       return ( sqrt( (x-500.f)*(x-500.f) + (y-500.f)*(y-500.f) ) < 100.f ) ? 15.f: 10.0f;
    };

	virtual float endSimulation() { return (float) 200; };

    virtual BoundaryType getBoundaryType(BoundaryEdge edge) { return OUTFLOW; };

    /** Get the boundary positions
     *
     * @param i_edge which edge
     * @return value in the corresponding dimension
     */
    float getBoundaryPos(BoundaryEdge i_edge) {
       if ( i_edge == BND_LEFT )
         return (float)0;
       else if ( i_edge == BND_RIGHT)
         return (float)1000;
       else if ( i_edge == BND_BOTTOM )
         return (float)0;
       else
         return (float)1000;
    };
};

/**
 * Scenario "Bathymetry Dam Break":
 * uniform water depth, but elevated bathymetry in the centre of the domain
 */
class SWE_BathymetryDamBreakScenario : public SWE_Scenario {

  public:

    float getBathymetry(float x, float y) { 
       return ( std::sqrt( (x-500.f)*(x-500.f) + (y-500.f)*(y-500.f) ) < 50.f ) ? -255.f: -260.f;
    };
    
	virtual float endSimulation() { return (float) 15; };

    virtual BoundaryType getBoundaryType(BoundaryEdge edge) { return OUTFLOW; };

    /** Get the boundary positions
     *
     * @param i_edge which edge
     * @return value in the corresponding dimension
     */
    float getBoundaryPos(BoundaryEdge i_edge) {
       if ( i_edge == BND_LEFT )
         return (float)0;
       else if ( i_edge == BND_RIGHT)
         return (float)1000;
       else if ( i_edge == BND_BOTTOM )
         return (float)0;
       else
         return (float)1000;
    };

    /**
     * Get the water height at a specific location.
     *
     * @param i_positionX position relative to the origin of the bathymetry grid in x-direction
     * @param i_positionY position relative to the origin of the bathymetry grid in y-direction
     * @return water height (before the initial displacement)
     */
    float getWaterHeight( float i_positionX,
                          float i_positionY ) {
      return (float) 260;
    }
};

/**
 * Scenario "Sea at Rest":
 * flat water surface ("sea at rest"), 
 * but non-uniform bathymetry (id. to "Bathymetry Dam Break")
 * test scenario for "sea at rest"-solution 
 */
class SWE_SeaAtRestScenario : public SWE_Scenario {

  public:

    float getWaterHeight(float x, float y) { 
       return ( sqrt( (x-0.5)*(x-0.5) + (y-0.5)*(y-0.5) ) < 0.1f ) ? 9.9f: 10.0f;
    };
    float getBathymetry(float x, float y) { 
       return ( sqrt( (x-0.5)*(x-0.5) + (y-0.5)*(y-0.5) ) < 0.1f ) ? 0.1f: 0.0f;
    };

};

/**
 * Scenario "Splashing Pool":
 * intial water surface has a fixed slope (diagonal to x,y)
 */
class SWE_SplashingPoolScenario : public SWE_Scenario {

  public:

    float getBathymetry(float x, float y) {
       return -250.f;
    };

    float getWaterHeight(float x, float y) {
    	return 250.0f+(5.0f-(x+y)/200);
    };

	virtual float endSimulation() { return (float) 15; };

    /** Get the boundary positions
     *
     * @param i_edge which edge
     * @return value in the corresponding dimension
     */
    float getBoundaryPos(BoundaryEdge i_edge) {
       if ( i_edge == BND_LEFT )
         return (float)0;
       else if ( i_edge == BND_RIGHT)
         return (float)1000;
       else if ( i_edge == BND_BOTTOM )
         return (float)0;
       else
         return (float)1000;
    };

};

/**
 * Scenario "Splashing Cone":
 * bathymetry forms a circular cone
 * intial water surface designed to form "sea at rest"
 * but: elevated water region in the centre (similar to radial dam break)
 */
class SWE_SplashingConeScenario : public SWE_Scenario {

  public:

    float getWaterHeight(float x, float y) { 
       float r = sqrt( (x-0.5f)*(x-0.5f) + (y-0.5f)*(y-0.5f) );
       float h = 4.0f-4.5f*(r/0.5f);

       if (r<0.1f) h = h+1.0f;

       return (h>0.0f) ? h : 0.0f;
    };

    float getBathymetry(float x, float y) { 
       float r = sqrt( (x-0.5f)*(x-0.5f) + (y-0.5f)*(y-0.5f) );
       return 1.0f+9.0f*( (r < 0.5f) ? r : 0.5f);
    };
    
    float waterHeightAtRest() { return 4.0f; };
    float endSimulation() { return 0.5f; };

    virtual BoundaryType getBoundaryType(BoundaryEdge edge) { return OUTFLOW; };
};

#endif
