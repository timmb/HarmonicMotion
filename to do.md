A list (MVP)
============

X Test JSON
X-- get rid of .pipeline in filename
X- Save doesn't work
X- Parameters saving wrong values
X- Missing node from save (use created nodes)
X- Open doesn't work
X crash on New
X need escape to cancel patch cord
X delete nodes
X BUG CPU usage goes up after cancelling patch cord creation
X BUG patching from outlets mouse doesn't respond
X BUG added node while pipeline is running - node is not properly started
X- Refactor partial patch cords to not use WidgetPatchCord
X- Node prototypes no longer create WidgetNode - now done via model
- Test save and load
- Expression node
- OSCeleton OSC output
- Refactor Factory to be part of Pipeline
-- Pipeline should be able to take a position as part of node creation args
-- WidgetNode should only be created on request from Pipeline.

B list (Essential for NIME)
===========================

- HUD display
-- Add HUD display to outlet context menu
- Webcam
-- Create webcam node
- Blob tracking node
- Midi out
- Windows build
- Kinect v2
- check for save on exit
- IA Filter nodes
- Save location with widgets
- BUG Context menu appearing in wrong place

C Spec described in NIME paper
===========
- Thread all nodes
- Custom OSC out
- Leap

D Desirable
===========
- Add start/stop to GUI
- Refactor to improve MVC: nodes should only be created by pipeline. WidgetNode, etc should be renamed ViewNode, etc.
- BUG Parameters initializing to garbage values
- BUG cannot create nodes in right area
- BUG mouse wheel over spin boxes
- BUG node names cannot be reused (move sNodeNamesInUse to pipeline and make the unique check done when node added to pipeline)
- REFACTOR make only pipeline able to create nodes
- REFACTOR move headers to hm/
- REFACTOR hm should be submodule of gui
- OSC in/out between instances
- Info display
- Auto calibrate multiple Kinects