rootnode = gr.node('root')

red = gr.material({1.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)
blue = gr.material({0.0, 0.0, 1.0}, {0.1, 0.1, 0.1}, 10)
green = gr.material({0.0, 1.0, 0.0}, {0.1, 0.1, 0.1}, 10)
white = gr.material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10)
wood1 = gr.material({0.871, 0.722, 0.529}, {0.1, 0.1, 0.1}, 10)
wood2 = gr.material({0.545, 0.353, 0.169}, {0.1, 0.1, 0.1}, 10)
cyan = gr.material({0.0431, 0.871, 0.929}, {0.0, 0.0, 1.0}, 10)

rootnode:translate(0.0, 0.0, -7.0)

topTorso = gr.mesh('sphere', 'topTorso')
rootnode:add_child(topTorso)
topTorso:set_material(wood1)
topTorso:scale(0.6,0.7,0.6)
topTorso:translate(0.0, 0.0, -7.0)

neckJoint = gr.joint('neckJoint', {90, 90, 0}, {0,0,0})
topTorso:add_child(neckJoint)
neckJoint:scale(0.2,0,2,0.2)
neckJoint:translate(0.0, 0.7, -7.0)

neck = gr.mesh('sphere', 'neck')
neckJoint:add_child(neck)
neck:scale(0.2, 0.3, 0.2)
neck:translate(0.0, 0.75, -7.0)
neck:set_material(wood2)

headJoint = gr.joint('headJoint', {80, 80, 0}, {90,0,180})
neck:add_child(headJoint)
headJoint:scale(0.2,0,2,0.2)
headJoint:translate(0.0, 0.76, -7.0)

head = gr.mesh('sphere', 'head')
headJoint:add_child(head)
head:scale(0.4, 0.6, 0.4)
head:translate(0.0, 1.4, -7.0)
head:set_material(wood1)

hLine1 = gr.mesh('cube', 'hLine1')
head:add_child(hLine1)
hLine1:scale(0.035, 0.4, 0.035)
hLine1:translate(0.0, 1.4, -6.6)
hLine1:set_material(wood2)

hLine2 = gr.mesh('cube', 'hLine2')
head:add_child(hLine2)
hLine2:scale(0.4, 0.035, 0.035)
hLine2:translate(0.0, 1.4, -6.6)
hLine2:set_material(wood2)

waist = gr.mesh('sphere', 'waist')
topTorso:add_child(waist)
waist:set_material(wood2)
waist:scale(0.5,0.25,0.5)
waist:translate(0.0, -0.6, -7.0)

botTorso = gr.mesh('sphere', 'botTorso')
waist:add_child(botTorso)
botTorso:set_material(wood1)
botTorso:scale(0.5,0.5,0.5)
botTorso:translate(0.0, -1.0, -7.0)

rShoulder = gr.mesh('sphere', 'rShoulder')
topTorso:add_child(rShoulder)
rShoulder:set_material(wood2)
rShoulder:scale(0.25,0.25,0.25)
rShoulder:translate(-0.55, 0.4, -7.0)

rShoulderJt = gr.joint('rShoulderJt', {0, 90, 360}, {0,0,0})
rShoulder:add_child(rShoulderJt)
rShoulderJt:scale(0.25,0.25,0.25)
rShoulderJt:translate(-0.55, 0.4, -7.0)

rBicep = gr.mesh('sphere', 'rBicep')
rShoulderJt:add_child(rBicep)
rBicep:set_material(wood1)
rBicep:scale(0.2,0.5,0.2)
rBicep:translate(-0.6, -0.2, -7.0)

rElbow = gr.mesh('sphere', 'rElbow')
rBicep:add_child(rElbow)
rElbow:set_material(wood2)
rElbow:scale(0.2,0.2,0.2)
rElbow:translate(-0.6, -0.6, -7.0)

rElbowJt = gr.joint('rElbowJt', {0, 0, 90}, {0,0,0})
rElbow:add_child(rElbowJt)
rElbowJt:scale(0.2,0.2,0.2)
rElbowJt:translate(-0.6, -0.6, -7.0)

rForearm = gr.mesh('sphere', 'rForearm')
rElbowJt:add_child(rForearm)
rForearm:set_material(wood1)
rForearm:scale(0.15,0.6,0.15)
rForearm:translate(-0.6, -1, -7.0)

rWrist = gr.mesh('sphere', 'rWrist')
rForearm:add_child(rWrist)
rWrist:set_material(wood2)
rWrist:scale(0.15,0.15,0.15)
rWrist:translate(-0.6, -1.5, -7.0)

rWristJt = gr.joint('rWristJt', {0, 0, 90}, {0,0,0})
rWrist:add_child(rWristJt)
rWristJt:scale(0.15,0.15,0.15)
rWristJt:translate(-0.6, -1.5, -7.0)

rHand = gr.mesh('sphere', 'rHand')
rWristJt:add_child(rHand)
rHand:set_material(wood1)
rHand:scale(0.1,0.25,0.1)
rHand:translate(-0.6, -1.7, -7.0)

lShoulder = gr.mesh('sphere', 'lShoulder')
topTorso:add_child(lShoulder)
lShoulder:set_material(wood2)
lShoulder:scale(0.25,0.25,0.25)
lShoulder:translate(0.55, 0.4, -7.0)

lShoulderJt = gr.joint('lShoulderJt', {0, 90, 360}, {0,0,0})
lShoulder:add_child(lShoulderJt)
lShoulderJt:scale(0.25,0.25,0.25)
lShoulderJt:translate(0.55, 0.4, -7.0)

lBicep = gr.mesh('sphere', 'lBicep')
lShoulderJt:add_child(lBicep)
lBicep:set_material(wood1)
lBicep:scale(0.2,0.5,0.2)
lBicep:translate(0.6, -0.2, -7.0)

lElbow = gr.mesh('sphere', 'lElbow')
lBicep:add_child(lElbow)
lElbow:set_material(wood2)
lElbow:scale(0.2,0.2,0.2)
lElbow:translate(0.6, -0.6, -7.0)

lElbowJt = gr.joint('lElbowJt', {0, 0, 90}, {0,0,0})
lElbow:add_child(lElbowJt)
lElbowJt:scale(0.2,0.2,0.2)
lElbowJt:translate(0.6, -0.6, -7.0)

lForearm = gr.mesh('sphere', 'lForearm')
lElbowJt:add_child(lForearm)
lForearm:set_material(wood1)
lForearm:scale(0.15,0.6,0.15)
lForearm:translate(0.6, -1, -7.0)

lWrist = gr.mesh('sphere', 'lWrist')
lForearm:add_child(lWrist)
lWrist:set_material(wood2)
lWrist:scale(0.15,0.15,0.15)
lWrist:translate(0.6, -1.5, -7.0)

lWristJt = gr.joint('lWristJt', {0, 0, 90}, {0,0,0})
lWrist:add_child(lWristJt)
lWristJt:scale(0.15,0.15,0.15)
lWristJt:translate(0.6, -1.5, -7.0)

lHand = gr.mesh('sphere', 'lHand')
lWristJt:add_child(lHand)
lHand:set_material(wood1)
lHand:scale(0.1,0.25,0.1)
lHand:translate(0.6, -1.7, -7.0)

rHip = gr.mesh('sphere', 'rHip')
botTorso:add_child(rHip)
rHip:set_material(wood2)
rHip:scale(0.25,0.25,0.25)
rHip:translate(-0.3, -1.4, -7.0)

rHipJt = gr.joint('rHipJt', {0, 90, 180}, {0,0,0})
rHip:add_child(rHipJt)
rHipJt:scale(0.25,0.25,0.25)
rHipJt:translate(-0.3, -1.5, -7.0)

rThigh = gr.mesh('sphere', 'rThigh')
rHipJt:add_child(rThigh)
rThigh:set_material(wood1)
rThigh:scale(0.25,0.6,0.25)
rThigh:translate(-0.3, -2, -7.0)

rKnee = gr.mesh('sphere', 'rKnee')
rThigh:add_child(rKnee)
rKnee:set_material(wood2)
rKnee:scale(0.2,0.2,0.2)
rKnee:translate(-0.3, -2.5, -7.0)

rKneeJt = gr.joint('rKneeJt', {90, 90, 0}, {0,0,0})
rKnee:add_child(rKneeJt)
rKneeJt:scale(0.2,0.2,0.2)
rKneeJt:translate(-0.3, -2.5, -7.0)

rShin = gr.mesh('sphere', 'rShin')
rKneeJt:add_child(rShin)
rShin:set_material(wood1)
rShin:scale(0.2,0.6,0.2)
rShin:translate(-0.3, -3, -7.0)

rAnkle = gr.mesh('sphere', 'rAnkle')
rShin:add_child(rAnkle)
rAnkle:set_material(wood2)
rAnkle:scale(0.15,0.15,0.15)
rAnkle:translate(-0.3, -3.5, -7.0)

rAnkleJt = gr.joint('rAnkleJt', {90, 90, 0}, {0,0,0})
rAnkle:add_child(rAnkleJt)
rAnkleJt:scale(0.15,0.15,0.15)
rAnkleJt:translate(-0.3, -3.5, -7.0)

rFoot = gr.mesh('sphere', 'rFoot')
rAnkleJt:add_child(rFoot)
rFoot:set_material(wood1)
rFoot:scale(0.15,0.15,0.3)
rFoot:translate(-0.3, -3.7, -6.8)

lHip = gr.mesh('sphere', 'lHip')
botTorso:add_child(lHip)
lHip:set_material(wood2)
lHip:scale(0.25,0.25,0.25)
lHip:translate(0.3, -1.4, -7.0)

lHipJt = gr.joint('lHipJt', {0, 90, 180}, {0,0,0})
lHip:add_child(lHipJt)
lHipJt:scale(0.25,0.25,0.25)
lHipJt:translate(0.3, -1.5, -7.0)

lThigh = gr.mesh('sphere', 'lThigh')
lHipJt:add_child(lThigh)
lThigh:set_material(wood1)
lThigh:scale(0.25,0.6,0.25)
lThigh:translate(0.3, -2, -7.0)

lKnee = gr.mesh('sphere', 'lKnee')
lThigh:add_child(lKnee)
lKnee:set_material(wood2)
lKnee:scale(0.2,0.2,0.2)
lKnee:translate(0.3, -2.5, -7.0)

lKneeJt = gr.joint('lKneeJt', {90, 90, 0}, {0,0,0})
lKnee:add_child(lKneeJt)
lKneeJt:scale(0.2,0.2,0.2)
lKneeJt:translate(0.3, -2.5, -7.0)

lShin = gr.mesh('sphere', 'lShin')
lKneeJt:add_child(lShin)
lShin:set_material(wood1)
lShin:scale(0.2,0.6,0.2)
lShin:translate(0.3, -3, -7.0)

lAnkle = gr.mesh('sphere', 'lAnkle')
lShin:add_child(lAnkle)
lAnkle:set_material(wood2)
lAnkle:scale(0.15,0.15,0.15)
lAnkle:translate(0.3, -3.5, -7.0)

lAnkleJt = gr.joint('lAnkleJt', {90, 90, 0}, {0,0,0})
lAnkle:add_child(lAnkleJt)
lAnkleJt:scale(0.15,0.15,0.15)
lAnkleJt:translate(0.3, -3.5, -7.0)

lFoot = gr.mesh('sphere', 'lFoot')
lAnkleJt:add_child(lFoot)
lFoot:set_material(wood1)
lFoot:scale(0.15,0.15,0.3)
lFoot:translate(0.3, -3.7, -6.8)


return rootnode