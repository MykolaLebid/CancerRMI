// Copyright 2017 Lebid Mykola all rights reserved

#ifndef SAMPLE_PROCESSING_H_INCLUDED
#define SAMPLE_PROCESSING_H_INCLUDED

#include <stdio.h>
#include <math.h>

#include <fstream>
#include <iostream>
#include <stdexcept>  // out_of_range

#include <vector>

#include "classes.h"



extern vector<Genotype*> genotypes;

// contains: max, min borders in x, y, z directions;
//           sweeps in x (del_1), y (del_2), z (del_3)
//           directions of the evolutionary population
//struct PopulationRange {
//    int min_x;  int max_x;
//    int min_y;  int max_y;
//    int min_z;  int max_z;
//    int del_1;  int del_2;  int del_3;
//};


// takes all cells in Rectangular cuboid @param Probe Pr and
// print them in a file with a name @param char *name;
// creates prob;
// write description of parameters or better to write structure for parameters
//float TakeProbe(char *name, DeltaProbeArea & pop_range,
//                 ofstream & file_tree_comparison,
//                 float max_dist_for_prob);
//float TakeProbe(Parameters & Pars);
void DoesInferenceAnalyze(Parameters & pars);
// takes an probe (sample) to build a tree
//
//void SetEtalonProbe(char *file_name_part, ProbeArea &,
//                    unsigned int threshold_num_cells);
void InitProbeArea(Parameters & pars);
void SetEtalonProbe(Parameters);
///////////////////////////////////////////////////////////////////////////////
// structure CompareCells
// support sort algo for probe_cells by length of genotype sequence(of muts)
struct CompareCells {
inline bool operator() (const Cell & a,const Cell & b) {
    unsigned int size_a = genotypes.at(a.gen)->sequence.size();
    unsigned int size_b = genotypes.at(b.gen)->sequence.size();
    if (size_a != size_b || size_a == 0){
        return (size_a < size_b);
    } else return (genotypes[a.gen]->sequence.at(size_a-1) <
                 genotypes[b.gen]->sequence.at(size_b-1));
};
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// structure CompareCells
// support unique algo for sorted probe_cells by deletion the same cells
struct CompareCellsEq {
    inline bool operator() (const Cell & a, const Cell & b) {
        unsigned int size_a= genotypes.at(a.gen)->sequence.size();
        unsigned int size_b= genotypes.at(b.gen)->sequence.size();
        unsigned int last_a = -1;
        unsigned int last_b = -1;
        if (size_a !=0 ) last_a = genotypes.at(a.gen)->sequence.at(size_a-1);
        if (size_b !=0 ) last_b = genotypes.at(b.gen)->sequence.at(size_b-1);
        if ((size_a == size_b) && (last_a == last_b)) {
            return true;
        } else return false;
    };
};
///////////////////////////////////////////////////////////////////////////////
// structure CompareCells
// supports exclusion algorithm
// adds number of cells in unique genotype after exclusion
struct CompareGenotypesEq {
    inline bool operator() ( Genotype * a, Genotype * b){
        unsigned int size_a= a->sequence.size();
        unsigned int size_b= b->sequence.size();
        unsigned int last_a = -1;
        unsigned int last_b = -1;
        if (size_a !=0 ) last_a = a->sequence.at(size_a-1);
        if (size_b !=0 ) last_b = b->sequence.at(size_b-1);
        if ((size_a == size_b) && (last_a == last_b)) {
            a->number++; //b->number=2;
            return true;
        } else return false;
    };
};/////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// structure included_genotype
// support unique algo for exclusion probe_cells by deletion the same cells
struct included_genotype {
inline bool operator() ( Genotype * a, Genotype * b){
    unsigned int size_a= a->sequence.size();
    unsigned int size_b= b->sequence.size();
    int last_a = -1;
    int last_b = -1;
    if (size_a !=0 ) last_a = a->sequence.at(size_a-1);
    if (size_b !=0 ) last_b = b->sequence.at(size_b-1);
    if ((size_a == size_b) && (last_a == last_b)) {
        a->number++; //b->number=2;
        return true;
    } else return false;
};
};/////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//Structure MutGenotypeConnection
// we use this  node structure after:
//1) sorting genotype by length @function UniqueGenotypes();
//2) after exclusions of inclusions @function GenotypesWithoutInclusions()//
//example:
//Genotype B= probe_genotypes[6].sequence=" p 51 p 789 p 3465 p 12456"
//Genotype A= probe_genotypes[7].sequence=" p 51 p 789 p 3465 d 4570 p 10020"
// -> mut_genotypes_connections[6].num_genotype=7
// -> mut_genotypes_connections[6].num_mutation=2
//Stub {-1, -1}; // Stub of vector mut_genotypes_connections
struct MutGenotypeConnection {
  // genotype A to which we attach correspondent (smaller) genotype B
  int num_genotype;
  // the place in line genotype A  where we attach the genotype B
  int num_mutation;
};/////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Structure MutationNode
// we use this node structure for:
// 1) comparison of trees;
// 2) all mutations is located in one vector and they are arranged
//    by the depth of mute
struct MutationNode {
    int abs_mut; // absolute number of this mutation (-1 for the root mut)
    unsigned int depth; // distance to the root mutation (begins with 0!!!)
    int abs_father_mut; // (-1 for the root mut)
    int index_father_mut;  //index in mutation_vector(zero_mut=-1)
    unsigned int num_children_mut;
    unsigned int number_cells;  // number of cells
                                // with this mutation in the end of genotypes
};
///////////////////////////////////////////////////////////////////////////////


struct ProbePiece {
  vector <Cell> cell_vector;
  int indicator = 0;
  int piece_min_x_vec;
  ComparisonResults results;
};
///////////////////////////////////////////////////////////////////////////////
// ProbeMutProcessor
// Class take vector of sells and work with mutation
// Strong dependency
// TODO destroy dependency with outer genotype_vector (main)
class ProbeMutProcessor {
  public:
  //Constructor:
  //@param const vector <Cell> & - Cells in Probe
  ProbeMutProcessor();
  ProbeMutProcessor(const vector <Cell> &);
  ProbeMutProcessor(const vector <Cell> &, unsigned int threshold_numb_cells);
  ProbeMutProcessor(const vector <Cell> &,
                   const vector <Genotype *> & genotype_vector);  // TODO
  //Destructor:
  ~ProbeMutProcessor();

  //we use only mutation_vector() for this method
  unsigned int get_cell_number() const;
  unsigned int get_mut_number() const;
  unsigned int get_tree_mut_depth() const; // max
  unsigned int get_tree_mut_degree() const;
  //vector <MutationNode> GetMutationNode(unsigned int index) const;
  vector <unsigned int> get_depth_mut_vec() const;
  vector <unsigned int> get_degree_mut_vec() const;
  //finds mean num of muts in probe cells
  float get_mean_mut_num_cell() const;
  float get_mean_pass_b_w_non_elem_nodes() const;
  //TODO change to private
  /////////////////////////////////////////////////////////////////////////////
  private:
  // Member variables
  // main cells from outside:
  vector <Cell> probe_cells;
  vector <Cell> raw_probe_cells;
  vector <MutationNode> mutation_vector;
  //genotype of main cells
  vector <Genotype *> probe_genotypes;
  //@vector: see description of struc MutGenotypeConnection:
  vector <MutGenotypeConnection> mut_genotypes_connections; ///////////////////
  // @vector: needs for sub-genotypes
  // example: A and B in probe_genotypes
  // Genotype B= " p 51 p 789 p 3465 "
  // Genotype A= " p 51 p 789 p 3465 d 4570 p 10020"
  // then B is a sub-genotype of A
  vector <Genotype *> excluded_probe_genotypes;
  /////////////////////////////////////////////////////////////////////////////


  // Support functions for constructor initialization:
  void ConstructorInit();  // main body of constructor. There are
                           // sequence of functions in ConstructorInit():
  /////////////////////////////////////////////////////////////////////////////
  void SetCellsGenotypes();    //  saves sorted vector of genotypes to
                               //  probe_genotypes (sorting was made
                               //  indirectly by sorting of cells by
                               //  lengths of genotype in constructor)
  void UniqueProbeCells(); // left cells with diff.genotypes
  void UniqueGenotypes(); // leaves unique genotypes and
                          // write new number of cells w.r.t. a genotype

  void GenotypesWithoutInclusions();//after 1) void UniqueGenotypes();
  /////////////////////////////////////////////////////////////////////////////
  // @function
  // needs in constructor after 1) void UniqueGenotypes() and
  // 2) void GenotypesWithoutInclusions();
  // creats vector mut_genotypes_connections;
  // this vector describes tree like structure with preprocessed
  // probe_genotypes vector
  // for the longest genotype we find the longest intersection
  // with small and remember the place in mut_genotypes_connections
  void CreatMutGenotypesConnections();
  /////////////////////////////////////////////////////////////////////////////
  void CreatMutVector();
  int GetNumCellsForCurNode(int index_cur_genotype_mut, int abs_mut);
  int GetFatherMutIndexForCurNode( int index_cur_probe_genotype,
                                   int index_cur_genotype_mut,
                                   int initial_place);
  MutationNode GetCurrentMutNode( int index_cur_probe_genotype,
                                  int index_cur_genotype_mut,
                                  int initial_place);
  int GetNumChildrenCurNode( int index_cur_probe_genotype,
                             int index_cur_genotype_mut);
  void InitZeroMutationNode(MutationNode& zero_mutation);
    ///////////////////////////////////////////////////////////////////////////

  //Add functions

  //string get_string_format_mut(unsigned int number);
  //Accessor functions
  int get_number_of_intrsections(Genotype * ,Genotype *);

  // File functions
  friend std::ostream & operator<<(std::ostream & output_out, const
                                     ProbeMutProcessor & ProbeMutProcessor_);
  friend std::istream & operator>>(std::istream& s_in,
                                     ProbeMutProcessor & ProbeMutProcessor_);
  // different tree metrics
  friend float t_dist_deg_vs_dep(const ProbeMutProcessor &,
																 const ProbeMutProcessor &);  // dist_type = 0

  // dist_type = 1
  friend float t_dist_deg_vs_mean_pass_non_elem(const ProbeMutProcessor &,
																		            const ProbeMutProcessor &);
  // dist_type = 2
  friend float t_dist_deg_vs_mean_pass_to_cell (const ProbeMutProcessor &,
																		            const ProbeMutProcessor &);

	friend float t_dist_mean_pass_to_cell(const ProbeMutProcessor &,
																				const ProbeMutProcessor &);
  // additional functions for tree metrics
  friend float chi_square_depth(const ProbeMutProcessor &,
																const ProbeMutProcessor &);
  friend float chi_square_degree(const ProbeMutProcessor &,
																 const ProbeMutProcessor &);
  friend float dist_mean_pass_b_w_non_elem_nodes(const ProbeMutProcessor &,
                                                 const ProbeMutProcessor &);
  friend float dist_mean_pass_to_cell(const ProbeMutProcessor &,
																			const ProbeMutProcessor &);
protected:
  /////////////////////////////////////////////////////////////////////////////
  //Support functions for file writing
  void SaveProbeCells(const char *);
  void SaveGenotypes(char *, vector<Genotype*> &);
  void SaveMutGenotypeConnections(char*);
  void SaveMutVector(char*);
  void SaveGraph(char *);//create a tex file with simple dot graph construction
  /////////////////////////////////////////////////////////////////////////////

};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class AnalyzeFrec {
public:
    //Constructor
    AnalyzeFrec(vector <Cell> & A,unsigned int total_number_of_SNV,
              unsigned int num_of_div);
    //Destructor
    ~AnalyzeFrec();
    // Member variables
    unsigned int num_of_div;
    unsigned int total_number_of_SNV;
    vector <int> vec_num_mutations;
    vector <int> frec_in_div;
    //Support functions
    void add_one_genotype(Cell one_cell);
};
///////////////////////////////////////////////////////////////////////////////
#endif // SAMPLE_PROCESSING_H_INCLUDED