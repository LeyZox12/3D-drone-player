#include <math.h>
#include <iostream>
#include <algorithm>
#include <functional>
#include <chrono>
#include <future>
#include <thread>
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#define PI 3.141592654

using namespace std;

typedef Vector3 vec3;

class Rect
{
    public: 
        Rect()
        {

        }

        Rect(float x, float y, float z, float w, float h, float d)
        :x(x), y(y), z(z), w(w), h(h), d(d) 
        {
        }

        Rect(float x, float y, float z, float w, float h, float d, string texturePath)
        :x(x), y(y), z(z), w(w), h(h), d(d) 
        {
        }

        void setRect(float x, float y, float z, float w, float h, float d)
        {
            this->x = x;
            this->y = y;
            this->x = z;
            this->w = w;
            this->h = h;
            this->d = d;
        }

        void setColor(Color col)
        {
            this->color = col;
        }

        void draw()
        {
            DrawCube(vec3(x, y, z), w, h, d, color);
        }

        float x = 0.f;
        float y = 0.f;
        float z = 0.f;
        float w = 0.f;
        float h = 0.f;
        float d = 0.f;
        Color color;
};

class Point
{
    public:
        Point()
        {

        }

        Point(vec3 pos, float radius, bool isStatic, bool shouldCollide, float friction)
        {
            this -> pos = pos;
            this -> oldPos = pos;
            this -> radius = radius;
            this -> acc = vec3(0, 0, 0);
            this -> isStatic = isStatic;
            this -> shouldCollide = shouldCollide;
            this -> gravityScale = 1.f;
            this -> friction = friction;
        }

        void setPos(vec3 pos, bool overrideStatic)
        {
            if(isStatic && !overrideStatic) return;
            this -> pos = pos;
        }

        void move(vec3 offset, bool overrideStatic)
        {
            if(isStatic && !overrideStatic) return;
            oldPos = pos;
            pos += offset;
        }

        void setGravityScale(float scale)
        {
            this->gravityScale = scale;
        }

        void setAcc(vec3 acc)
        {
            this -> acc = acc;
        }

        void addAcc(vec3 offset)
        {
            acc += offset;
        }

        void setShouldCollide(bool shouldCollide)
        {
            this -> shouldCollide = shouldCollide;
        }

        void setIsStatic(bool isStatic)
        {
            this -> isStatic = isStatic;
        }

        void setFriction(float friction)
        {
            this -> friction = friction;
        }

        void setOldPos(vec3 pos)
        {
            oldPos = pos;
        }

        void setRadius(float radius)
        {
            this -> radius = radius;
        }

        void setColor(Color color)
        {
            displayColor = color;
        }

        Color getColor()
        {
            return displayColor;
        }

        vec3 getPos()
        {
            return pos;
        }

        vec3 getOldPos()
        {
            return oldPos;
        }

        float getRadius()
        {
            return radius;
        }

        float getFriction()
        {
            return friction;
        }

        float getGravityScale()
        {
            return gravityScale;
        }

        vec3 getAcc()
        {
            return acc;
        }

        bool getIsStatic()
        {
            return isStatic;
        }

        bool getShouldCollide()
        {
            return shouldCollide;
        }
    private:
        bool isStatic;
        bool shouldCollide;
        vec3 pos;
        vec3 oldPos;
        vec3 acc;
        float radius;
        float friction;
        float gravityScale;
        Color displayColor;

};

class PhysicConstraint
{
    public:
        PhysicConstraint()
        {

        }

        PhysicConstraint(int i1, int i2, int type, float distance, bool visible)
        {
            indexes = make_pair(i1, i2);
            this -> type = type;
            this -> distance = distance;
            this -> visible = visible;
        }

        void setIndex1(int index)
        {
            indexes.first = index;
        }

        void setIndex2(int index)
        {
            indexes.second = index;
        }

        void setType(int type)
        {
            this -> type = type;
        }

        void setDistance(float dist)
        {
            distance = dist;
        }

        void setVisible(bool visible)
        {
            this -> visible = visible;
        }

        void setIndexes(pair<int, int> indexes)
        {
            this -> indexes = indexes;
        }

        pair<int, int> getIndexes()
        {
            return indexes;
        }

        int getType()
        {
            return type;
        }

        float getDist()
        {
            return distance;
        }

        bool getVisible()
        {
            return visible;
        }
    private:
        pair<int, int> indexes;
        int type = 0;
        float distance = 0;
        bool visible = true;
};

class PointEngine
{
    public:
        /*void addPoint(vec2 pos, bool isStatic, bool shouldCollide, float radius, float friction)
        {
            points.emplace_back(Point(pos, radius, isStatic, shouldCollide, friction));
            points[points.size()-1].setGravityScale(1.f);
        }

        void addPoint(vec2 pos, bool isStatic, bool shouldCollide, float radius, float friction, function<vector<any>(OnUpdateContext ctx)> onUpdate)
        {
            points.emplace_back(Point(pos, radius, isStatic, shouldCollide, friction, onUpdate));
            points[points.size()-1].setGravityScale(1.f);
        }*/


        void addPoint(vec3 pos, bool isStatic, bool shouldCollide, float radius, float friction, Color displayColor)
        {
            Point instance = Point(pos, radius, isStatic, shouldCollide, friction);
            instance.setColor(displayColor);
            points.push_back(instance);
        }

        void removePoint(int index)
        {
            if(index < points.size())
            {
                vector<PhysicConstraint> buffer = constraints;

                for(int c = 0; c < constraints.size(); c++)
                {
                    if(constraints[c].getIndexes().first > index)
                        constraints[c].setIndexes(make_pair(constraints[c].getIndexes().first-1, constraints[c].getIndexes().second));
                    if(constraints[c].getIndexes().second > index)
                        constraints[c].setIndexes(make_pair(constraints[c].getIndexes().first, constraints[c].getIndexes().second-1));
                    //cout << "1:" << constraints[c].getIndexes().first << ", 2:"<< constraints[c].getIndexes().second << endl;
                }removeConstraints(index);
                points.erase(points.begin() + index);

            }
        }

        void removeConstraints(int index)
        {
            vector<int> toRemove = {};
            for(int i = 0; i < constraints.size(); i++)
            {
                if(constraints[i].getIndexes().first == index || constraints[i].getIndexes().second == index)
                    toRemove.push_back(i);
            }
            sort(toRemove.begin(), toRemove.end(), greater<int>());
            for(auto& r : toRemove)
                constraints.erase(constraints.begin() + r);
        }

        Rect& getRect(int index)
        {
            return rectangles[index];
        }

        //void addRect(Rect<int> rect)
        //{
        //    rectangles.push_back(Rectangle(rect));
        //}

        void addRectangle(float x, float y, float z, float w, float h, float d) 
        {
            rectangles.emplace_back(Rect(x, y, z, w, h, d));
        }

        void removeRectangle(int index)
        {
            if(index < rectangles.size())
                rectangles.erase(rectangles.begin() + index);
        }

        void addConstraint(int i1, int i2, int constraintType, float constraintDistance)
        {
            if(constraintDistance == 0)
            {
                vec3 diff = points[i1].getPos() - points[i2].getPos();
                constraintDistance = hypot(hypot(diff.x, diff.y), diff.z);
            }
            constraints.emplace_back(PhysicConstraint(i1, i2, constraintType, constraintDistance, true));
        }

        void addConstraint(int i1, int i2, int constraintType, float constraintDistance, bool isVisible)
        {
            for(auto& c : constraints)
                if(c.getIndexes() == make_pair(i1, i2)) return;

            if(constraintDistance == 0)
            {
                vec3 diff = points[i1].getPos() - points[i2].getPos();
                constraintDistance = hypot(hypot(diff.x, diff.y), diff.z);
            }
            constraints.emplace_back(PhysicConstraint(i1, i2, constraintType, constraintDistance, isVisible));
        }

        void removeConstraint(int index)
        {
            /*if(index < constraints.size())
            {
                constraintDistance.erase(constraintDistance.begin() + index);
                constraintType.erase(constraintType.begin() + index);
                constraints.erase(constraints.begin() + index);
            }*/
        }

        PhysicConstraint getConstraint(int index)
        {
            return constraints[index];
        }

        int getPointIndexAtPos(vec3 point)
        {/*
            for(int i = 0; i < points.size(); i++)
            {
                vec2 diff = points[i].getPos() - point;
                float dist = hypot(diff.x, diff.y);
                if(dist < points[i].getRadius()) return i;
            }*/
            return -1;
        }

        Point& getPoint(int index)
        {
            return points[index];
        }

        int getPointCount()
        {
            return points.size();
        }

        int getConstraintCount()
        {
            return constraints.size();
        }

        void updatePointPos(float dt)
        {
            int index = 0;
            for(auto& p : points)
            {
                vec3 currPos = p.getPos();
                vec3 currOldPos = p.getOldPos();
                vec3 currAcc = p.getAcc();
                p.addAcc(vec3(0, -9.8 * p.getGravityScale(), 0));
                p.setAcc(p.getAcc() * 0.99f);
                vec3 newPos = currPos * 2.0f - currOldPos + currAcc * dt * dt;
                p.setOldPos(currPos);
                p.setPos(newPos, false);
                index++;
            }
        }
        void applyConstraints(int substeps, float dt)
        {
            Point *p1, *p2;
            vec3 fixed1, fixed2, dir;
            PhysicConstraint currConstraint;
            float dist;
            float diff;
            for(int _ = 0; _ < substeps; _++)
                for(int c = 0; c < constraints.size(); c++)
                {
                    currConstraint = constraints[c];
                    p1 = &points[currConstraint.getIndexes().first];
                    p2 = &points[currConstraint.getIndexes().second];
                    dir = p1->getPos() - p2->getPos();
                    dist = hypot(hypot(dir.x, dir.y), dir.z);
                    dir = Vector3Normalize(dir);
                    diff = (dist - currConstraint.getDist());
                    float staticRatio = 0.5;
                    if(p1->getIsStatic() || p2->getIsStatic())
                        staticRatio += 0.5;
                    switch(currConstraint.getType())
                    {
                        case(0):
                            if(dist >= constraints[c].getDist())
                                break;
                            fixed1 = p2->getPos() + dir * diff * -staticRatio;
                            fixed2 = p1->getPos() - dir * diff * staticRatio;
                            p1->setPos(fixed1, false);
                            p2->setPos(fixed2, false);
                            break;
                        case(1):
                            if(dist <= constraints[c].getDist())
                                break;
                            fixed1 = p2->getPos() + dir * diff * -staticRatio;
                            fixed2 = p1->getPos() - dir * diff * staticRatio;
                            p1->setPos(fixed1, false);
                            p2->setPos(fixed2, false);
                            break;
                        case(2):
                            if(dist == constraints[c].getDist())
                                break;
                            fixed1 = p1->getPos() - dir * diff * staticRatio;
                            fixed2 = p2->getPos() + dir * diff * staticRatio;
                            p1->setPos(fixed1, false);
                            p2->setPos(fixed2, false);
                            break;
                        case(3):
                        fixed1 = p1->getPos() - dir * diff * 0.05f * dt;
                            fixed2 = p2->getPos() + dir * diff * 0.05f * dt;
                            p1->setPos(fixed1, false);
                            p2->setPos(fixed2, false);
                            break;
                    }
                }
        }

        enum Collisions
        {
            NONE,
            UP,
            DOWN,
            LEFT,
            RIGHT
        };

        void applyCollisions(int substeps)
        {
            for(int _ = 0; _ < substeps; _++)
            {
                int pIndex = 0;
                for(int j = 0; j < points.size(); j++)
                {
                    Point& p = points[j];
                    /*for(int i = 0; i < points.size(); i++)
                        if(p.getShouldCollide() && points[i].getShouldCollide() && i != pIndex)
                        {
                            Point *p2 = &points[i];
                            float r1 = p.getRadius();//radius point 1
                            float r2 = p2->getRadius();//radius point 2
                            float rc = r1 + r2; //radius combined
                            vec2 dir = p.getPos() - p2->getPos();
                            float dist = hypot(dir.x, dir.y);
                            if(dist > 0.f && dist < rc){
                                dir /= dist;
                                float difference = rc - dist;
                                float ratio1 = r2 / r1;
                                float ratio2 = r1 / r2;
                                vec2 fixed1, fixed2;
                                if(ratio1 < ratio2)
                                {
                                    fixed1 = p.getPos() - (rc - dist) * ratio1 * -dir;
                                    fixed2 = p2->getPos() - (rc - dist) * (1.f - ratio1) * dir;
                                }
                                else if(ratio2 < ratio1)
                                {
                                    fixed1 = p.getPos() - (rc - dist) * (1.f - ratio2) * -dir;
                                    fixed2 = p2->getPos() - (rc - dist) * ratio2 * dir;
                                }
                                else
                                {

                                    fixed1 = p.getPos() - (rc - dist) * 0.5f * -dir;
                                    fixed2 = p2->getPos() - (rc - dist) * 0.5f * dir;
                                }
                                if(dist < rc)
                                {
                                    p.setPos(fixed1, false);
                                    p2->setPos(fixed2, false);
                                }
                            }
                        }*/
                    for(int r = 0; r < rectangles.size(); r++)
                    {
                        /*int collision = circleRectCollision(rectangles[r], p);
                        if(collision == UP)
                        {
                            p.setPos({p.getPos().x, rectangles[r].getRect().position.y - p.getRadius(), p.getPos().z}, false);
                            float vel = p.getOldPos().x - p.getPos().x;
                            p.setAcc({vel * p.getFriction(), 0});
                        }
                        else if(collision == RIGHT)
                        {
                            p.setPos({rectangles[r].getRect().position.x + rectangles[r].getRect().size.x + p.getRadius(), p.getPos().y}, false);
                            p.setAcc({0, p.getAcc().y, p.getAcc.z});
                        }
                        else if(collision == LEFT)
                        {
                            p.setPos({rectangles[r].getRect().position.x - p.getRadius(), p.getPos().y}, false);
                            p.setAcc({0, p.getAcc().y});
                        }*/
                        if(p.getPos().y < -1.5f)
                        {
                            p.setPos(vec3(p.getPos().x, -1.5f, p.getPos().z), false);
                            vec3 acc = p.getAcc();
                            p.setAcc(vec3(acc.x, 0.0, acc.z));
                        }
                    }
                }
            }
        }

        void applyCollisionsPart(int substeps, int start, int end)
        {
        }

        int circleRectCollision(Rect rect, Point p)
        {/*
            float bx = p.getPos().x;
            float by = p.getPos().y;
            float br = p.getRad();
            float rx = rect.x;
            float ry = rect.y;
            float sx = rect.w;
            float sy = rect.h;
            int collision = NONE;
            vector<float> distances = {
                                        abs(by + br - ry),
                                        abs(by -br - (ry + sy)),
                                        abs(bx + br - rx),
                                        abs(bx - br - (rx + sx))
                                    };
            int collisionIndex = distance(distances.begin(), min_element(distances.begin(), distances.end()));
            if(distances[collisionIndex] < br && bx + br > rx && bx - br < rx + sx && by + br > ry && by - br < ry + sy)
                collision = collisionIndex + 1;
            return collision;*/
            return -1;
        }

        void display(Color color)
        {
            for(auto& point : points)
            {
                DrawSphere(point.getPos(), point.getRadius(), color);
            }
            for(auto& rect : rectangles)
            {
                rect.draw();
            }
            /*
            for(auto& c : constraints)
            {
                if(c.getVisible())
                {
                    Point p1 = points[c.getIndexes().first];
                    Point p2 = points[c.getIndexes().second];
                    line[0].color = p1.getColor();
                    line[1].color = p2.getColor();
                    line[0].position = p1.getPos();
                    line[1].position = p2.getPos();
                    window.draw(line);
                }
            }
            for(auto& r : rectangles)
            {
                r.setColor(color);
                r.draw(window);
            }*/
        }
    private:
        vector<Point> points;
        vector<PhysicConstraint> constraints;
        vector<Rect> rectangles;
        vector<thread> pool;
};
