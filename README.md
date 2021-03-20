## Project title
Angry Nerds!

## Team members
Selena Sun, Kevin Yang, James Zheng

## Project description
Our project is an arcade-like Angry Birds game, where the player uses a physical slingshot to control the force and trajectory of the bird (in this case, the wonderful TAs and instructors of our class) and hit a target on screen! Depending on the angle and force with which the player pulls back the bird on the slingshot, as measured by an inertial measurement unit and a force-sensing resistor, the bird’s trajectory is modified and plotted with the graphics library.

## Member contribution

Kevin: set up graphics (plotting trajectory on-screen with force and angle measurements with kinematics and drawing images of staff members on-screen), set up game logic (with countdown, keyboard, graphics, and difficulty selection components)

James: set up and programmed force-sensing resistor along with ADC, implemented/tested function for generating random target, various alterations to combine hardware with software

Selena: sourced and assembled hardware (ie., making extension wires, designing integrated sensor system), designed/tested/implemented IMU processing sequence (with James's help!)

## References
Cite any references/resources that inspired/influenced your project. 
If your submission incorporates external contributions such as adopting 
someone else's code or circuit into your project, be sure to clearly 
delineate what portion of the work was derivative and what portion is 
your original work.

For our cosine function, we used a simple approximation of cos(x) using a 6th-order Taylor polynomial.
Citation: https://web.eecs.utk.edu/~azh/blog/cosine.html


## Self-evaluation
How well was your team able to execute on the plan in your proposal?  
Any trying or heroic moments you would like to share? Of what are you particularly proud: the effort you put into it? the end product? 
the process you followed? what you learned along the way? Tell us about it!

Overall, our team was able to accomplish our goal of replicating an Angry Birds-esque game using a real slingshot. Heroic moments include spending days on implementing an amazing suite of graphics functions (Kevin), setting up the slingshot and a majority of the hardware (Selena), and staying up the night before project presentations to integrate hardware with software (James and Selena). We were able to learn the difficulties that come with using sensors such as IMUs and force-sensing resistors, the difficulties that go into combining hardware with software, as well as the workflow of using Git and Github to code collaboratively. 

From choosing a hardware setup from a myriad of recommendations, we learned that a good idea well-executed is better than a perfect idea with mediocre execution. From grappling with the often inaccurate values from the IMU/force sensor, we learned to apply software solutions to hardware problems. Lastly, we learned that integration of hardware and software requires thorough and detailed communication (and that debugging issues takes longer than one may expect).

## Photos
You are encouraged to submit photos/videos of your project in action. 
Add the files and commit to your project repository to include along with your submission.

Plotting various trajectories!
<img width="909" alt="plotting trajectories graphics" src="https://user-images.githubusercontent.com/6740161/111843188-58398980-88c6-11eb-8a62-5438e9bd59eb.png">

Issues with scaling to the correct framebuffer size in calculating the initial velocity vector and trajectory:
<img width="869" alt="math errors graphics" src="https://user-images.githubusercontent.com/6740161/111843241-6b4c5980-88c6-11eb-90d2-5b4f77651a7d.png">

Issues with loading the image buffer:
<img width="511" alt="image buffer issues" src="https://user-images.githubusercontent.com/6740161/111843267-756e5800-88c6-11eb-92fa-2f2d846da3a8.png">
<img width="908" alt="image buffer issues 2" src="https://user-images.githubusercontent.com/6740161/111843291-815a1a00-88c6-11eb-8e69-4bd1ada02cc1.png">

Sort-of working images of instructors -- sorry Peter and Liana!
<img width="406" alt="displaying images of instructors" src="https://user-images.githubusercontent.com/6740161/111843300-88812800-88c6-11eb-83ac-c32a9cfcdf36.png">

Top-down view of hardware setup
