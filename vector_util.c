# def cos(45) 0.707107

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
  float mag1 = mic1_mag - mic3_mag;
  float mag2 = mic4_mag - mic2_mag;
  xval = (mag1 - mag2) * cos(45);
  yval = (mag1 + mag2) * cos(45);
  struct vect retVect = create_vector(xval, yval);
  return retVect;
}
