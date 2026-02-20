@mainpage Introduction

<img src="bennu.jpg" alt="asteroid 101955 Bennu" style="float:right; padding: 10px;" width="480" height="270">The Planetary Shape Model and Ray Tracing System (PSMRTS) enables the efficient utilization of shape models for applications such as planetary mapping. PSMRTS is a result of experience gained in mapping asteroid 101955 Bennu for NASA's <a href="https://science.nasa.gov/mission/osiris-rex/" target="_blank" rel="noopener">OSIRIS-REx</a> (OREx) mission.
 
The United States Geological Survey's (USGS) <a href="https://isis.astrogeology.usgs.gov/9.0.0/index.html" target="_blank" rel="noopener">Integrated Software for Imagers and Spectrometers</a> (ISIS3) package was used for most OSIRIS-REx mapping tasks. While the publicly available version of `ISIS3` provided basic support for mapping small, irregularly shaped bodies, it lacked functionality to accurately, quickly, and efficiently map them at high resolution.

With `ISIS3` as a foundation, the University of Arizona (UA) OSIRIS-REx imaging team has developed `PSMRTS` to provide that missing functionality. 'PSMRTS' supports ray tracing with one or multiple high precision shape models (more than 3 million vectors each) in a variety of formats for image orthorectification and other geometric computations. The`PSMRTS` library is being made available to the scientific community and will integrated back into a formal public `ISIS` release for general use and application.

