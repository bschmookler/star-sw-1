//StiDetectorTreeBuilder.h
//M.L. Miller (Yale Software)
//07/01

/*! \class StiDetectorTreeBuilder
  StiDetectorTreeBuilder is a utility class that uses objects it gets from two
  factories
  to build a full model of the STAR detector material.  StiDetectorTreeBuilder
  is the
  class responsible for actually organizing the StiDetector objects into a tree
  structure.
  As such, it uses the utility class StiDetectorBuilder to generate StiDetector
  objects, and
  then these objects are organized as belonging to an
  StiCompositeTreeNode<StiDetector>
  object.  This is all accomplished via the call to build().
  <p>
  The general flow of execution is as follows.  First, in the constructor of
  StiDetectorTreeBuilder, the member mDetectorBuilder is set to point to an
  instance
  of StiCodedDetectorBuilder created on the heap.  Once a call to build() is
  made, the
  tree is assembled by looping on detectors that are generated by
  mDetectorBuilder.  Each
  detector object returned by the mDetectorBuilder is then hung on the tree by
  a call to
  addToTree which calls hangWhere().  By using mDetectorBuilder polymorphically,
  StiDetectorTreeBuilder becomes extremely flexible.  That is, it does not
  care how the
  StiDetector objects are created (e.g., from root macro, data base, or geant).
  However,
  to really take advantage of this flexibility one should remove ownership of
  mDetectorBuilder from StiDetectorTreeBuilder and instead set the polymorphic
  pointer
  by hand before a call to build().  This is work to be done.
  
  \author M.L. Miller (Yale Software)
  \warning There is <b>some</b> internal protection against build being called
  more then once.  See the documenation for the build() method.
  \warning Member of type StiDetectoBuilder* defaults to
  StiCodedDetectorBuilder.
 */

/** \example StiDetectorTreeBuilder_ex.cxx
 */

#ifndef StiDetectorTreeBuilder_HH
#define StiDetectorTreeBuilder_HH

#include <vector>
using std::vector;
#include "StiCompositeTreeNode.h"

#include "StiFactoryTypes.h"
#include "StiObjectFactoryInterface.h"

class StiDetectorBuilder;

class StiDetectorTreeBuilder
{
public:

    ///Default Contstructor
    StiDetectorTreeBuilder();

    ///Default Destructor
    virtual ~StiDetectorTreeBuilder();

    ///Build the Detector model.
    /*! There is <b>some</b> internal protection against building more than one instance of the
      detector model.  That is, StiDetectorTreeBuilder stores a pointer to the root of
      the tree that it builds.  build() first checks that this pointer is null.  If not,
      the call to build() will return 0.  However, once the StiDetectorTreeBuilder instance
      that originally built the tree goes out of scope, so does the stored pointer to the
      root, and thus the protection becomes impossible.
      Additionaly, the root of the tree is assumed to be owned by the nodefactory.
    */
    data_node* build(StiObjectFactoryInterface<StiDetectorNode>* nodefactory,
		     StiObjectFactoryInterface<StiDetector>* detfactory);
    
private:
    ///Iterate over the detector objects served by StiDetectorBuilder.
    void loopOnDetectors();

    ///Assemble detector objects into tree.
    void buildRoot();

    ///Actually hang an individual detector object on the tree.
    void addToTree(StiDetector*);

    ///Decide where to hang the detector object on the tree.
    data_node* hangWhere(data_node* parent, StiOrderKey_t order, 
                         string& keystring);

    ///Store a pointer to the root of the tree.
    data_node* mroot;
    ///This object is assumed <b>not</b> to be owned by this class

    ///Store a pointer to the factory of tree nodes.
    ///This is used for internal convenience.  This object is <b>not</b> owned by this class.
    StiObjectFactoryInterface<StiDetectorNode>* mnodefactory;
    
    ///Store a pointer to the factory of detector objects.
    StiObjectFactoryInterface<StiDetector>* mdetfactory;
    ///This is used for internal convenience.  This object is <b>not</b> owned by this class.

    ///Store a pointer to the StiDetectorBuilder instance
    StiDetectorBuilder *mDetectorBuilder;
    ///This object <b>is</b> owned by this class.   This object is polymorphic.  That is,
    /// StiDetectorBuilder is a pure virtual class and thus can construct only instances
    /// of derived types.  The current default is the following: \n
    /// <code>mDetectorBuilder = new StiCodedDetectorBuilder()</code>

    ///We store a pointer to the current region in the tree (e.g., mid rapidity)
    data_node* mregion;
    
};

#endif
