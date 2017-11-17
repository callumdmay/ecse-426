float IIR(float InputArray[3], float outputArray[3]) {
  //filter coefficients
  float coeffiecients[5] = {
    0.2,
    0.2,
    0.2,
    0.2,
    0.2
  };
  //filter equation
  outputArray[0] = coeffiecients[0] * InputArray[0] + coeffiecients[1] * InputArray[1] + coeffiecients[2] * InputArray[2];
  return outputArray[0];

}

//shift input and output array values for IIR filter calculation, set input index 0 to current accelerometer value
void filterValues(float rawValues[3], float filteredValues[3]) {
	//static array declerations
  static float arrayX[3];
  static float arrayY[3];
  static float arrayZ[3];
  static float outputX[3];
  static float outputY[3];
  static float outputZ[3];
	//set input[n-2] to previous input[n-1]
  arrayX[2] = arrayX[1];
  arrayY[2] = arrayY[1];
  arrayZ[2] = arrayZ[1];
	//set input[n-1] to previous input[n]
  arrayX[1] = arrayX[0];
  arrayY[1] = arrayY[0];
  arrayZ[1] = arrayZ[0];
	//set input[n] to accelerometer register raw values
  arrayX[0] = rawValues[0];
  arrayY[0] = rawValues[1];
  arrayZ[0] = rawValues[2];
	//set output[n-2] to previous output[n-1], then output[n-1] to previous output[n]
  outputX[2] = outputX[1];
  outputX[1] = outputX[0];
	
  outputY[2] = outputY[1];
  outputY[1] = outputY[0];

  outputZ[2] = outputZ[1];
  outputZ[1] = outputZ[0];

  //assign filtered values to array
  filteredValues[0] = IIR(arrayX, outputX);
  filteredValues[1] = IIR(arrayY, outputY);
  filteredValues[2] = IIR(arrayZ, outputZ);

}
