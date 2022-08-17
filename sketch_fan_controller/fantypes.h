struct signal {
  boolean power;
  int duration; // microseconds
};

const signal SHORT_ON { true, 400 };
const signal SHORT_OFF { false, 500 };
const signal LONG_ON { true, 850 };
const signal LONG_OFF { false, 950 };
const signal REST { false, 11000 };

// Complete message is 50 items
// Preamble uses 32
// Message content uses 10
// Postamble uses 8
// Another example online showed message of 16 and postamble of 2, but the last
// 6 of the messages was all the same. Adjust if needed.

signal sig_preamble [] {
  SHORT_ON, LONG_OFF,
  SHORT_ON, LONG_OFF,
  SHORT_ON, SHORT_OFF,
  LONG_ON, SHORT_OFF,

  LONG_ON, SHORT_OFF,
  LONG_ON, SHORT_OFF,
  LONG_ON, LONG_OFF,
  SHORT_ON, LONG_OFF,

  SHORT_ON, SHORT_OFF,
  LONG_ON, SHORT_OFF,
  LONG_ON, LONG_OFF,
  SHORT_ON, LONG_OFF,

  SHORT_ON, LONG_OFF,
  SHORT_ON, LONG_OFF,
  SHORT_ON, SHORT_OFF,
  LONG_ON, SHORT_OFF
};

signal sig_postamble [] {
  LONG_ON, LONG_OFF,
  SHORT_ON, SHORT_OFF,
  LONG_ON, LONG_OFF,
  SHORT_ON, REST
};

signal sig_light [] {
  // Start message
  LONG_ON, LONG_OFF,
  SHORT_ON, LONG_OFF,
  SHORT_ON, SHORT_OFF,
  LONG_ON, LONG_OFF,
  
  SHORT_ON, SHORT_OFF
};

signal sig_power [] {
  // Start message
  LONG_ON, SHORT_OFF,
  LONG_ON, SHORT_OFF,
  LONG_ON, SHORT_OFF,
  LONG_ON, LONG_OFF,

  SHORT_ON, SHORT_OFF
};

signal sig_speed_1 [] {
  // Start message
  LONG_ON, LONG_OFF,
  SHORT_ON, SHORT_OFF,
  LONG_ON, SHORT_OFF,
  LONG_ON, SHORT_OFF,

  LONG_ON, SHORT_OFF
};

signal sig_speed_2 [] {
  // Start message
  LONG_ON, SHORT_OFF,
  LONG_ON, LONG_OFF,
  SHORT_ON, SHORT_OFF,
  LONG_ON, SHORT_OFF,

  LONG_ON, SHORT_OFF
};

signal sig_speed_3 [] {
  // Start message
  LONG_ON, LONG_OFF,
  SHORT_ON, LONG_OFF,
  SHORT_ON, SHORT_OFF,
  LONG_ON, SHORT_OFF,

  LONG_ON, SHORT_OFF
};
