X indicated something has been completed.

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
X- Test save and load
x- WidgetNode should only be created on request from Pipeline.
x- BUG patchcord square prevents clicking on widget beneath
x BUG node parameters in gui not initializing to correct value
x BUG parameter values not loading on open
x BUG enumeration values not loading on open
x BUG parameters not going invisible
x BUG patch cords appear in wrong place when scrolled down


B list (Essential)
===========================

x HUD display
x Save location with widgets
x new nodes appear in right place
x- Pipeline should be able to take a position as part of node creation args
x BUG adding patchcord moves node
x Webcam
x- Create webcam node
x- Blob tracking node
x- multiple datatypes on single renderer in correct order
x- sceneMeta from webcam used for 2d rendering
x- BUG loading file does not connect patchcords properly if destination inlet has changing name
x- reconnect patch cords when node characteristics change
x Make changing name on widget actually change name
x Ability to delete patch cords
x- BUG dragging on labels doesn't move node
x- Check if pipeline's actually dirty before asking to save changes
x- BUG Context menu appearing in wrong place
x- renderer to display 1 dimensional data alongisde 3d skeleton position info
x- line skeleton renderer
x- data carries an id value - a skeleton is then a list of 3d points, with joints matching the id, overall id is user id. processes retain the id allowing derived data to be matched up with its source.
x-- adjust BaseData class.
x-- adjust operators
x- adjust renderer
x-- adjust OSC
x- test new rendering code
x- Change render views to be consistent
x- IA filter process
x- test new IA filter node
x- BUG reloading file with renderer causes renderer to stop rendering
x- BUG something going wrong with loading file, ghost patch cords. maybe something to do with eventdispatcher.
x- BUG ghost patch cords when changing renderer
x- tooltips for inlet descriptions
x- 1d renderer limits
x- BUG saving files mixing type and name
x- BUG patchcords not moving when dragging nodes
- Midi out
- make default 1d renderer limits smaller

- Create nodes to make IA dynamic output
x-- mix between parameters
x-- moving average
x-- BUG Moving average calculates value wrong
x-- TODO: test speed and acceleration
x-- velocity
x-- speed
x-- peak decay
-- standard deviation
x-- select skeleton from scene
x-- select joint
x-BUG dragging nodes when scrolled to the right
x-BUG string parameter widget doesn't load after json load
x-BUG decay rate is incorrect (decaying too quickly)
x- Increate step size on spin boxes

- popup help box for expression node
- remember last open/save directory

- REFACTOR: Skeleton3d should derive from ListPoint3d and only carry one set of coordinates.
-- NodeKinect should have 3 outlets: joints, jointsProjective and confidence

- Expression node wishlist
x- add support for vector literals to expression node
-- support for 2d vector literals
-- add support for variables
x- allow some elements of vector literals to be variables/inputs
x- enforce not adding scalars to vectors in grammar
-- multiple statements
-- maths operators (cos, sin, etc)
-- bigger box to enter text
-- error reporting
-- display on widget if expression is invalid
-- display any evaluation errors on widget
-- subscript into vectors with v.x
-- history of inputs with i1_20
-- subscript into lists, including using joint id's with ., [] for index
-- if / else
-- comparison operators
-- vector operators: dot, magnitude

- new nodes:
-- delay
-- power curve
-- compression curve

x- make sure all operators update timestamp correctly
- operators should take min confidence from two operands
- Windows build
- multiple renderers in single node

/- render view for Value data and 1d list
- OSCeleton OSC output
- Thread all nodes
- Custom OSC out
- Leap
- Kinect v2


D Desirable
===========
- kinect should send empty scene
- report errors within nodes
- make inlet show when bad data is being received
- fold up parameters in widget
- nicer UI for tweaking parameters
- full screen render view
- sort node prototypes into groups
- console in application
- menu to set debug level for console
X- update Qt to try to get rid of combobox bug
- rename VALUE to SCALAR
- Make inlet's provideNewData take move constructor only
- allow strings to be enumerations
- inlet callback should run in same thread as stepProcessing
- Refactor Factory to be part of Pipeline
- point cloud data
- Add start/stop to GUI
x Refactor to improve MVC: nodes should only be created by pipeline. WidgetNode, etc should be renamed ViewNode, etc.
x BUG Parameters initializing to garbage values
- BUG cannot create nodes in right area
- BUG mouse wheel over spin boxes
x BUG node names cannot be reused (move sNodeNamesInUse to pipeline and make the unique check done when node added to pipeline)
- REFACTOR make only pipeline able to create nodes
- REFACTOR move headers to hm/
x REFACTOR Scene3d to inherit from List<Skeleton3d>
- OSC in/out between instances
- Info display
- Auto calibrate multiple Kinects
- Name uniqueness should be controlled by pipeline, not factory
- undo/redo
- select multiple objects
- copy/paste
- multiple pipelines open
- use weak pointers in more places 
--draw a picture of strong references to check there's no cycles
--create static reigstration of construction and destruction to ensure no objects lingering
- OPTIMISATION nodes should only do work if an outlet is connected

