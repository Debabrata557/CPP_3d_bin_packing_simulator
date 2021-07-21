
#ifndef config
#define config

const int BIN_WIDTH = 120;
const int BIN_HEIGHT = 120;
const int BIN_LENGTH = 180;
const int MAX_BOX_LENGTH = 60;
const int MAX_BOX_WIDTH = 60;
const int MAX_BOX_HEIGHT = 60;
const int MIN_BOX_LENGTH = 30;
const int MIN_BOX_WIDTH = 30;
const int MIN_BOX_HEIGHT = 30;
const int CONTROLLER_TOLERANCE = 0;

const int SCALING_FACTOR = 1;  // effective bin_width/height/length = bin_width/height/length *(1/scaling_factor)..Make sure the effective bin_width/height/length are integers
#endif
