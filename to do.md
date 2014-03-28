A list (MVP)
============

- Test JSON
X-- get rid of .pipeline in filename
-- Save doesn't work
-- Parameters saving wrong values
X- Missing node from save (use created nodes)
X- Open doesn't work
X crash on New
- IA Filter nodes
- Expression node
- OSCeleton OSC output
- BUG patching from outlets mouse doesn't respond
- need escape to cancel patch cord
- delete nodes

B list (Essential for NIME)
===========================

- HUD display
- Webcam
- Blob tracking
- Midi out
- Windows build
- Kinect v2
- check for save on exit

C Spec described in NIME paper
===========
- Thread all nodes
- Custom OSC out
- Leap

D Desirable
===========
- Refactor to improve MVC: nodes should only be created by pipeline. WidgetNode, etc should be renamed ViewNode, etc.
- BUG cannot create nodes in right area
- BUG mouse wheel over spin boxes
- BUG node names cannot be reused (move sNodeNamesInUse to pipeline and make the unique check done when node added to pipeline)
- REFACTOR make only pipeline able to create nodes
- REFACTOR move headers to hm/
- OSC in/out between instances
- Info display
- Auto calibrate multiple Kinects