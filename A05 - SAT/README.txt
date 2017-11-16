Extra Challenge Stuff:
I generate the 15 separation planes based on an up (normal) vector and a right vector
The right vector just has to be perpendicular to the up vector.  Direction doesn't matter since I need to draw two planes so it is visible from both sides
To get the forward vector I cross(up, right) and then to get the reversed vector I cross(right, up)

I put them all into a mat3(right, forward, up) to create the plane's rotation matrix

Then I calculate a translation matrix based on the object position and the half width

Then I scale the plane up

The planes should never clip if they are being drawn for one of the 6 local axes, but they occasionally clip if the plane is the result 
of a generated cross product axis.  I didn't know how to solve that since I generate all 9 axes instead of hard coding them like in the book.

Check out the AddSeparationPlane function in MyRigidBody.cpp for my implementation.