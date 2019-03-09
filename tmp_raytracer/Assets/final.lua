gold = gr.material({0.9, 0.8, 0.4}, {1, 1, 1}, 25)
mirror = gr.material({0.9, 0.8, 0.4}, {1, 1, 1}, 25)
grass = gr.material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 25)
blue = gr.material({0.7, 0.6, 1}, {0.0, 0.0, 0.0}, 25)
red = gr.material({0.502, 0, 0}, {0, 0, 0}, 25)
maroon = gr.material({0.502, 0, 0}, {1, 1, 1}, 25, 0, true)
glass = gr.material({0, 0, 0}, {1, 1, 1}, 5, 1.5)
frostedGlass = gr.material({0, 0, 0}, {1, 1, 1}, 25, 1.5, true)
white = gr.material({1, 1, 1}, {0.0, 0.0, 0.0}, 25)

scene = gr.node('scene')
scene:rotate('X', 20)
scene:translate(6, -6, -14)

-- the floor
plane = gr.mesh( 'plane', 'Assets/planeFloor.obj' )
scene:add_child(plane)
plane:set_material(grass)
plane:scale(15, 15, 15)

-- the table
plane = gr.mesh( 'plane', 'Assets/planeDarkwood.obj' )
scene:add_child(plane)
plane:set_material(mirror)
plane:scale(4, 4, 4)
plane:translate(-3, 9, 9.5)

-- opposite wall
plane = gr.mesh( 'plane', 'Assets/plane.obj' )
scene:add_child(plane)
plane:set_material(blue)
plane:scale(15, 15, 14)
plane:rotate('X', 90)
plane:translate(0,0,-15)

-- adjacent wall
plane = gr.mesh( 'plane', 'Assets/plane.obj' )
scene:add_child(plane)
plane:set_material(blue)
plane:scale(15, 1, 8)
plane:rotate('Z', -90)
plane:translate(-15,0,-8)

-- christmas tree
chTree = gr.mesh( 'chTree', 'Assets/lowtree.obj' )
scene:add_child(chTree)
chTree:set_material(grass)
chTree:rotate('Y',-10)
chTree:scale(3.5, 3.5, 3.5)
chTree:translate(-3.5, 0, -10)

-- snow globe sphere
globe = gr.sphere('globe')
scene:add_child(globe)
globe:set_material(glass)
globe:scale(0.7,0.7,0.7)
globe:translate(-6.3,9.7,7.6)

--[[
-- snow globe tree
tree = gr.mesh( 'tree', 'Assets/tree.obj' )
scene:add_child(tree)
tree:set_material(white)
tree:scale(0.06, 0.06, 0.06)
tree:translate(-6.25,9.32,7.6)
]]
-- snow globe tree alternate
tree = gr.mesh( 'tree', 'Assets/lowtree.obj' )
scene:add_child(tree)
tree:set_material(white)
tree:scale(0.2, 0.2, 0.2)
tree:translate(-6.3,9.38,7.6)

base = gr.cube( 'base')
scene:add_child(base)
base:set_material(white)
base:scale(0.1, 0.1, 0.1)
base:translate(-6.35,9.4,7.7)

-- snow globe base
base = gr.cube( 'base')
scene:add_child(base)
base:set_material(white)
base:scale(1, 0.4, 1)
base:translate(-6.78,9,7)

-- gem
gem = gr.mesh('gem', 'Assets/dodeca.obj')
scene:add_child(gem)
gem:scale(0.4,0.4,0.4)
gem:rotate('X',20)
gem:set_material(frostedGlass)
gem:translate(-4.7,9.4,7.6)

-- ornament
orn = gr.sphere('orn')
scene:add_child(orn)
orn:rotate('X', -10)
orn:rotate('Z', -15)
orn:set_material(maroon)
orn:scale(0.35,0.35,0.35)
orn:translate(-5.6,9.4,8.8)

orn_ring = gr.sphere('orn_ring')
orn:add_child(orn_ring)
orn_ring:set_material(gold)
orn_ring:scale(1.02,0.7,1.02)

orn_tip = gr.cube('orn_tip')
orn:add_child(orn_tip)
orn_tip:set_material(gold)
orn_tip:scale(0.3,0.3,0.3)
orn_tip:translate(-0.15,0.9,-0.1)


-- ornament2
orn = gr.sphere('orn', {0,0.2,0})
scene:add_child(orn)
orn:rotate('Z', 100)
orn:rotate('Y', 50)
orn:set_material(maroon)
orn:scale(0.4,0.4,0.4)
orn:translate(-5.4,9.3,8.5)

orn_ring = gr.sphere('orn_ring')
orn:add_child(orn_ring)
orn_ring:set_material(gold)
orn_ring:scale(1.02,0.7,1.02)

orn_tip = gr.cube('orn_tip')
orn:add_child(orn_tip)
orn_tip:set_material(gold)
orn_tip:scale(0.3,0.3,0.3)
orn_tip:translate(-0.15,0.9,-0.1)

orn:translate(-2,-0.6,-1)

-- ornament3
o1 = gr.node('o1')
scene:add_child(o1)

orn = gr.sphere('orn')
scene:add_child(orn)
orn:set_material(maroon)
orn:scale(0.4,0.4,0.4)
orn:translate(-6.8,9.3,8.5)

orn_ring = gr.sphere('orn_ring')
orn:add_child(orn_ring)
orn_ring:set_material(gold)
orn_ring:scale(1.02,0.7,1.02)

orn:translate(4,0,-17)

o1:add_child(orn)
o1:translate(-3,-3,0)


-- The lights
l1 = gr.light({200,200,400}, {0.7, 0.7, 0.7}, {1, 0, 0})
l3 = gr.light({0,100,0}, {0.3, 0.3, 0.3}, {1, 0, 0})
l2 = gr.light({-8,15,5}, {0.4, 0.4, 0.4}, {1, 0, 0})

gr.render(scene, 'finalAlt.png',600, 600, 
	  {0, 0, 0,}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.9, 0.9, 0.9}, {l1,l3})
