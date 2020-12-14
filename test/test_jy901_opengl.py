#!/usr/bin/env python3
# coding: utf-8
# JY901 9-axis motion sensor demo
# Peng Shulin <trees_peng@163.com> 2020
from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *
from mcush import WitMotion, AppUtils
from scipy.spatial.transform import Rotation as R
        
angle_x, angle_y, angle_z = 0, 0, 0

VIEW = [-1.0, 1.0, -1.0, 1.0, 1.0, 10.0]  # left/right/bottom/top/near/far
EYE = [1.0, 1.0, 1.0]
AIM = [0.0, 0.0, 0.0]
UP = [0.0, 0.0, 1.0]
WIDTH, HEIGHT = 600, 600

def draw():
    glClear(GL_COLOR_BUFFER_BIT)
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    glFrustum(VIEW[0], VIEW[1], VIEW[2], VIEW[3], VIEW[4], VIEW[5])
    gluLookAt( EYE[0], EYE[1], EYE[2], AIM[0], AIM[1], AIM[2], UP[0], UP[1], UP[2] )
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()
    
    glBegin(GL_LINES)
    # red X axis
    glColor4f(1.0, 0.0, 0.0, 0.5)
    glVertex3f(0.0, 0.0, 0.0)
    glVertex3f(1.0, 0.0, 0.0)
    # green Y axis
    glColor4f(0.0, 1.0, 0.0, 0.5)
    glVertex3f(0.0, 0.0, 0.0)
    glVertex3f(0.0, 1.0, 0.0)
    # blue Z axis
    glColor4f(0.0, 0.0, 1.0, 0.5)
    glVertex3f(0.0, 0.0, 0.0)
    glVertex3f(0.0, 0.0, 1.0)
    glEnd()
    #print('draw', angle_x, angle_y, angle_z)
    glRotatef( angle_x, 1,0,0 )
    glRotatef( angle_z, 0,0,1 )
    glRotatef( angle_y, 0,1,0 )
    glutWireTeapot(0.5);
    glutSwapBuffers()

def keydown(key, x, y):
    if key == b'q':
        jy901_monitor.terminate()
        glutLeaveMainLoop() 

class JY901Monitor(AppUtils.Task):
    def target( self, args ):
        wt = WitMotion.JY901()
        wt.setRSW( ang=True, quat=False, acc=False, gyo=False, mag=False )
        #wt.setRSW( ang=False, quat=True, acc=False, gyo=False, mag=False )
        wt.setRate(0x07)
        while True:
            for p in wt.readPackets():
                if p.getType() == 'ang':
                    self.queue.put( ('ang', p.getData()[:3]) ) 
                elif p.getType() == 'quat':
                    self.queue.put( ('quat', p.getData()) ) 


def idle():
    global angle_x, angle_y, angle_z
    try:
        cmd, val = jy901_monitor.queue.get(timeout=0.05)#block=False)
    except AppUtils.Empty:
        return
    print(cmd, val)
    if cmd == 'ang':
        angle_x, angle_y, angle_z = val
        glutPostRedisplay()
    elif cmd == 'quat':
        q0,q1,q2,q3 = val
        angle_x, angle_y, angle_z = R.from_quat(val).as_euler('zyx',degrees=True)
        #print( angle_x, angle_y, angle_z )
        glutPostRedisplay()


if __name__ == "__main__":
    glutInit()
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_ALPHA)
    glutInitWindowSize(WIDTH, HEIGHT)
    glutCreateWindow('JY901 OpenGL Demo')
    glViewport(0, 0, WIDTH, HEIGHT)
    glEnable(GL_DEPTH_TEST)
    glDepthFunc(GL_LEQUAL)
    glutDisplayFunc(draw)
    glutKeyboardFunc(keydown)
    glutIdleFunc(idle)
    jy901_monitor = JY901Monitor( None )
    glutMainLoop()

