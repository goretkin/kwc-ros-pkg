#ifndef URDF_PARSER_
#define URDF_PARSER_

#include <string>
#include <vector>
#include <map>

/* Universal Robot Description File (URDF) parser */

class URDF
{
 public:
    
    struct Link
    {
	
	struct Geometry
	{
	    Geometry(void)
	    {
		type = UNKNOWN;
		size[0] = size[1] = size[2] = 0.0;
	    }
	    
	    enum
		{
		    UNKNOWN, SPHERE, BOX, CYLINDER, MESH
		}       type;
	    std::string name;
	    double      size[3];
	    std::string filename;
	};
	
	struct Joint
	{
	    Joint(void)
	    {
		axis[0] = axis[1] = axis[2] = 0.0;
		anchor[0] = anchor[1] = anchor[2] = 0.0;
		calibration[0] = calibration[1] = 0.0;
		limit[0] = limit[1] = 0.0;
		type = UNKNOWN;
	    }
	    
	    enum
		{
		    UNKNOWN, REVOLUTE, PRISMATIC, FLOATING
		}       type;
	    std::string name;
	    double      axis[3];
	    double      anchor[3];
	    double      limit[2];
	    double      calibration[2];
	};
	
	struct Collision
	{
	    Collision(void)
	    {
		xyz[0] = xyz[1] = xyz[2] = 0.0;
		rpy[0] = rpy[1] = rpy[2] = 0.0;
		geometry = new Geometry();
	    }
	    
	    ~Collision(void)
	    {
		if (geometry)
		    delete geometry;
	    }

	    std::string  name;	    
	    double       xyz[3];
	    double       rpy[3];
	    std::string  material;
	    Geometry    *geometry;
	};
	
	struct Inertial
	{
	    Inertial(void)
	    {
		mass = 0.0;
		com[0] = com[1] = com[2] = 0.0;
		inertia[0] = inertia[1] = inertia[2] = inertia[3] = inertia[4] = inertia[5] = 0.0;
	    }
	    std::string name;
	    double      mass;
	    double      inertia[6];
	    double      com[3];
	};
	
	struct Visual
	{
	    Visual(void)
	    {
		xyz[0] = xyz[1] = xyz[2] = 0.0;
		rpy[0] = rpy[1] = rpy[2] = 0.0;
		geometry = new Geometry();
	    }
	    
	    ~Visual(void)
	    {
		if (geometry)
		    delete geometry;
	    }

	    std::string  name;
	    double       xyz[3];
	    double       rpy[3];
	    std::string  material;
	    Geometry    *geometry;
	};
	
	Link(void)
	{
	    parent = NULL;
	    xyz[0] = xyz[1] = xyz[2] = 0.0;
	    rpy[0] = rpy[1] = rpy[2] = 0.0;
	    inertial  = new Inertial();
	    visual    = new Visual();
	    collision = new Collision();
	    joint     = new Joint();
	}
	
	virtual ~Link(void)
	{
	    if (inertial)
		delete inertial;
	    if (visual)
		delete visual;
	    if (collision)
		delete collision;
	    if (joint)
		delete joint;
	}
	
	Link              *parent;
	std::string        parentName;
	std::string        name;
	std::vector<Link*> children;
	
	Inertial          *inertial;
	Visual            *visual;
	Collision         *collision;
	Joint             *joint;
	
	double             rpy[3];
	double             xyz[3];
    };
    
    struct Sensor : public Link
    {
	
	Sensor(void)
	{
	}
	
	virtual ~Sensor(void)
	{
	}
	
	enum
	{
	    CAMERA
	}       type;
    };    
    
    explicit URDF(const char *filename = NULL)
    {   
	m_paths.push_back("");
	if (filename)
	    load(filename);
    }
    
    virtual ~URDF(void)
    {
	freeMemory();
    }
    
    virtual void clear(void);
    virtual bool load(const char *filename);

 protected:

    /* free the memory allocate in this class */
    void freeMemory(void);
    
    void  addPath(const char *filename);
    char* findFile(const char *filename);

    /* parse the URDF document */
    virtual bool parse(const void *data);
    virtual void ignoreNode(const void *data);
    
    std::string                  m_source;
    std::vector<std::string>     m_paths;
    
    std::string                  m_name;
    std::map<std::string, Link*> m_links; // contains sensors too (casted down)
    std::vector<Link*>           m_roots; // contains the links that are connected to the world (have no parent)    

    std::map<std::string, Link::Collision*> m_collision;
    std::map<std::string, Link::Joint*>     m_joints;
    std::map<std::string, Link::Inertial*>  m_inertial;
    std::map<std::string, Link::Visual*>    m_visual;
    std::map<std::string, Link::Geometry*>  m_geoms;

 private:
    
    /* utility functions for parsing */
    void loadLink(const void *data);
    void loadSensor(const void *data);
    void loadJoint(const void *data, Link::Joint *joint);
    void loadGeometry(const void *data, Link::Geometry *geometry);
    void loadCollision(const void *data, Link::Collision *collision);
    void loadVisual(const void *data, Link::Visual *visual);
    void loadInertial(const void *data, Link::Inertial *inertial);
    void defaultConstants(void);
    void getChildrenAndAttributes(const void *data, std::vector<const void *> &children, std::vector<const void *> &attributes) const;
    unsigned int loadValues(const void *data, unsigned int count, double *vals);
    std::string  extractName(std::vector<const void *> &attributes);    
    
    
    /* temporary storage for information during parsing; should not be used elsewhere */
    std::map<std::string, std::string> m_constants;  // constants 
    std::map<std::string, const void*> m_templates;  // templates
    std::vector<const void*>           m_stage2;     // xml nodes that should be processed after all templates and constants are read
    std::vector<void*>                 m_docs;       // pointer to loaded documents
    
};


#endif
