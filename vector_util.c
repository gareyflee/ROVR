#include "math.h"

#define COS45 0.70710678118

struct vect{
  float x;
  float y;
};

struct vect create_vector(float x, float y){
  struct vect v = {x,y};
  return v;
}

struct vect add_mic_values(float mic1_mag, float mic2_mag, float mic3_mag, float mic4_mag){
  // Microphones are numbered starting at the front of the board and moving clockwise
  const float mag1 = mic1_mag - mic3_mag;
  const float mag2 = mic4_mag - mic2_mag;
  const float xval = (mag1 - mag2) * COS45;
  const float yval = (mag1 + mag2) * COS45;
  const struct vect retVect = create_vector(xval, yval);
  return retVect;
}
