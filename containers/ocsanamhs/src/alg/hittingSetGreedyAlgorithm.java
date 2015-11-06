/*
 * Copyright (C) 2015 Vera-Licona Research Group
 * Author: Misagh Kordi
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 *
 * @author misagh.kordi
 */

import java.util.*;

public class hittingSetGreedyAlgorithm {

    /**
     * Shows largest cardinality of CIs to be searched by greedy algorithm.
     */
    private final int maxCIsize;

    /**
     * Shows largest number (of millions) of candidate CIs to be tested for each size of Cis by greedy algorithm.
     */
    private final double maxNumberofCandidate;

    /**
     * Shows final output of greedy Algorithm, each of List<Integer> shows one of hitting set solution that contains hashing map of all nodes on corresponding solution.
     */
    private final List<List<Integer>> CIset;

    /**
     * Shows all of paths between each of source nodes to each of target nodes, each of List<Integer> shows one path that contains hashing map of all nodes on this path.
     */
    private final List<List<Integer>> elementarypaths;

    /**
     * Shows ocsana score of each elementary nodes, elements order are corresponding to nodes on elementaryNodes vector
     */
    private final List<Float> ocsanaScore;

    /**
     * Shows hashing map of all nodes that appeared on at least one of elementary paths.
     */
    private final List<Integer> elementaryNodes;

    /**
     * Shows setscore of each elementary nodes in order of nodes on elementaryNodes vector.
     */
    private final int[] setscoreset;

    /**
     * Shows elementarypatharray for elementary nodes, that create by elementaryPathArray method.
     */
    private int[][] elementryPathArray;

    /**
     * Shows ocsana score for each CI set.
     */
    private float[] CIocsanascore;



    /**
     * Constructor
     * Initial value of  maxCIsize, maxNumberofCandidate, elementarypathes, elementarypathes, ocsanaScore and setscoreset
     *
     * @param maxCIsize, maxNumberofCandidate, elementarypathes, elementarypathes, ocsanaScore and setscoreset
     */
    public hittingSetGreedyAlgorithm( int maxCIsize,
                                      double maxNumberofCandidate,
                                      List<List<Integer>> elementarypaths,
                                      List<Float> ocsanaScore,
                                      int[] setscoreset,
                                      List<Integer> elementaryNodes) {

        this.elementarypaths = elementarypaths;
        this.maxCIsize = maxCIsize;
        this.maxNumberofCandidate =  maxNumberofCandidate;
        this.ocsanaScore = ocsanaScore;
        this.elementaryNodes = elementaryNodes;
        this.setscoreset = setscoreset;



        CIset = new ArrayList<>();
        initial();
    }



    /**
     * call sort method to sort elementarynodes nodes on their ocsana score
     * call elementaryPathArray method to create elementryPathArray table
     * call exceptionone, kEqualOne and to compute all of CIset
     * call CIsort to sort CIset based onocsana score
     *
     * @param
     */
    private void initial() {

        sort();
        elementaryPathArray();
        exceptionone();
        kEqualOne();
        allCIset();
        //CIsort();
    }



    /**
     * sort elementarynodes nodes and ocsanascore vector based on their ocsana score
     *
     * @param
     */
    public void sort() {
        int count = 0;

        int length = ocsanaScore.size();
        quickSort(0, length - 1);
    }

    /**
     * quickSort algorithm
     *
     *
     * @param
     */
    private void quickSort(int lowerIndex, int higherIndex) {

        int i = lowerIndex;
        int j = higherIndex;

        Float pivot = ocsanaScore.get(lowerIndex + (higherIndex - lowerIndex) / 2);

        while (i <= j) {

            while (ocsanaScore.get(i) < pivot){
                i++;
            }
            while (ocsanaScore.get(j) > pivot){
                j--;
            }
            if (i <= j) {

                exchangeNumbers(i, j);
                i++;
                j--;
            }
        }
        if (lowerIndex < j) {
            quickSort(lowerIndex, j);
        }
        if (i < higherIndex) {
            quickSort(i, higherIndex);
        }
    }

    /**
     * exchange two element on quickSort algorithm
     *
     *
     * @param
     */
    private void exchangeNumbers(int i, int j) {

        Float temp = ocsanaScore.get(i);
        int tempelementarynode;
        tempelementarynode = elementaryNodes.get(i);

        if (i < j | setscoreset[j] < setscoreset[i]) {

            ocsanaScore.set(i, ocsanaScore.get(j));
            ocsanaScore.set(j, temp);

            elementaryNodes.set(i, elementaryNodes.get(j));
            elementaryNodes.set(j, tempelementarynode);

        }
    }

    /**
     * Create elementryPathArray or elementary nodes
     *
     * @param
     */
    private void elementaryPathArray() {

        //Create elementaryPatharray with P+1 row and N+1 column, N is size of elementaryNodes and P is size of elementarypaths .
        int N = elementaryNodes.size();

        int p = elementarypaths.size();

        elementryPathArray = new int[p + 1][N + 1];

        //elementryPathArray[i][j] is one if i-th path has j-th elementary node, otherwise it is zero.
        for (int i = 0; i < p + 1; i++) {

            for (int j = 0; j < N + 1; j++) {
                elementryPathArray[i][j] = 0;
            }
        }

        for (int i = 0; i < elementarypaths.size(); i++) {

            for (int j = 1; j < elementarypaths.get(i).size()-1; j++) {
                elementryPathArray[i + 1][elementaryNodes.indexOf(elementarypaths.get(i).get(j))] = 1;
            }
        }
    }

    /**
     * check whether there is a path with length of 1 or not
     * @return true if there exist and false if not
     *
     * @param
     */
    private boolean exceptionone() {

        boolean excep = false;

        int count = 0;

        //Each one on each row corresponding to one elementary node so by count ing number of 1 at each row we can find length of path.
        for (int i = 1; i < elementryPathArray.length; i++) {

            count = 0;
            for (int j = 0; j < elementryPathArray[i].length; j++) {

                if (elementryPathArray[i][j] == 1) {
                    count++;
                }
            }

            //If there is just one element at row I that has value 1, it means this path has length zero so method return true.
            if (count == 1) {
                elementryPathArray[i][elementryPathArray[i].length - 1] = -1;
                excep = true;
            }
        }

        return excep;
    }

    /**
     * check whether there is a node that si on all of elementary paths or not
     * @return true if there exist and false if not
     *
     * @param
     */
    private void kEqualOne() {

        List<Integer> temp;
        int count = 0;

        int N = elementryPathArray[0].length - 1;

        for (int i = 0; i < N; i++) {

            count = 0;
            for (int j = 1; j < elementryPathArray.length; j++) {
                if (elementryPathArray[j][i] == 1) {
                    count++;
                }
            }

            if (count == elementryPathArray.length - 1) {
                temp = new ArrayList<>();
                temp.add(elementaryNodes.get(i));
                CIset.add(temp);
                elementryPathArray[0][i] = -2;
            }
        }
    }

    /**
     * Create Useed and based on it find solution of size 2 to MaxCI
     * @return
     *
     * @param
     */
    private void allCIset() {

        int count = 0;

        int NPrime = 0;

        int c2nprime = 0;

        int[][] H2;

        int[] elementarypathstemp = new int[elementarypaths.size()];

        List<Integer> NprimeOrder = new ArrayList<>();
        List<List<Integer>> U;
        List<Integer> tempvector;

        //Before start to run algorithm we first find how many elementary nodes in all of elementary paths can be candidate to add to CI set.
        //Based on kEqualOne method if one node is on all of path we remove that node from candidate and add it to MIset.so first, we count remain nodes.
        int N = elementryPathArray[0].length - 1;
        for (int i = 0; i < N; i++) {
            if (elementryPathArray[0][i] != -2) {
                NprimeOrder.add(elementaryNodes.get(i));
                count++;
            }
        }

        //If there is not any node candidate we cannot not add any nodes to CIset so algorithm finished
        NPrime = count;
        if (NPrime == 0 || NPrime == 1) {
            return;
        }


        c2nprime = (int) ((NPrime * (NPrime - 1)) / 2);

        H2 = new int[c2nprime + 1][NPrime + 1];

        for (int i = 0; i < c2nprime + 1; i++) {

            for (int j = 0; j < NPrime + 1; j++) {

                H2[i][j] = 0;
            }
        }

        int p = 1;
        int q = 1;
        int t = 0;

        while (true) {

            H2[p][t] = 1;
            if (q < NPrime) {

                H2[p][q] = 1;
                q++;
            } else {
                H2[p][t] = 0;
                t++;
                q = t + 1;

                p--;
            }
            if (p == c2nprime) {
                break;
            }
            p++;
        }

        int elementarypathhit = 0;
        int tempvalue1 = -1;
        int tempvalue2 = -1;
        List<List<Integer>> Useed = new ArrayList<>();
        List<Integer> CIset1;

        //For each row we check whether two chosen node can cover all of paths or not.If they cover all we add them as CIset size two otherwise we add them to Useed for create CIset size three or more.
        for (int i = 1; i < c2nprime + 1; i++) {

            tempvalue1 = -1;
            tempvalue2 = -1;
            elementarypathhit = 0;

            for (int j = 0; j < NPrime; j++) {
                if (H2[i][j] == 1) {

                    if (tempvalue1 == -1) {
                        tempvalue1 = j;
                    } else {
                        tempvalue2 = j;
                    }
                }
            }
            for (int j = 0; j < elementarypaths.size(); j++) {
                if (elementarypaths.get(j).contains(NprimeOrder.get(tempvalue1)) | elementarypaths.get(j).contains(NprimeOrder.get(tempvalue2))) {
                    elementarypathhit++;
                }
            }
            CIset1 = new ArrayList<>();
            //Elementarypathhit shows how many of paths covered by these two nodes.
            //If they cover all we add them as CIset size two. if (
            if (elementarypathhit == elementarypaths.size()) {
                CIset1.add(NprimeOrder.get(tempvalue1));
                CIset1.add(NprimeOrder.get(tempvalue2));
                CIset.add(CIset1);

            }
            //Otherwise we add them to Useed for create CIset size three or more. else { tempvector = new ArrayList<>(); tempvector.add(NprimeOrder.get(tempvalue1)); tempvector.add(NprimeOrder.get(tempvalue2)); Useed.add(tempvector);
            else {
                tempvector = new ArrayList<>();
                tempvector.add(NprimeOrder.get(tempvalue1));
                tempvector.add(NprimeOrder.get(tempvalue2));
                Useed.add(tempvector);

            }
        }

        int z = 0;
        boolean exit = false;
        for (int k = 3; k < maxCIsize && Useed.size() > 0 && Useed.get(Useed.size() - 1).size() < NprimeOrder.size() + 1 && exit == false; k++) {

            if (k == maxCIsize && exceptionone()) {
                break;
            }

            U = new ArrayList<>();

            //For each of node we check to see can we add it to Useed and create new CIset or not .
            for (int i = 0; i < NprimeOrder.size(); i++) {
                for (int j = 0; j < Useed.size(); j++) {
                    int x = 0;
                    for (int l = 0; l < CIset.size(); l++) {
                        //After that we check whether useed j has a subset on CIset with smaller size or not if it has we ignore that Useed element.
                        if (comapringtwopath(CIset.get(l), Useed.get(j))) {

                            x = 1;
                            break;
                        }
                    }

                    if (Useed.get(j).contains(NprimeOrder.get(i)) != true & x == 0) {
                        tempvector = new ArrayList<>();
                        for (int l = 0; l < Useed.get(j).size(); l++) {
                            tempvector.add(Useed.get(j).get(l));
                        }

                        tempvector.add(NprimeOrder.get(i));
                        x = 0;
                        // check whether useed j has a subset on CIset with smaller size or not if it has we ignore that Useed element.
                        for (int l = 0; l < CIset.size(); l++) {
                            if (comapringtwopath(CIset.get(l), tempvector)) {
                                x = 1;
                                break;
                            }
                        }
                        if (x == 0) {
                            int xx = 0;
                            //check whether candidate node is on chosen set of Useed (U) or not.
                            for (int l = 0; l < U.size(); l++) {
                                if (comapringtwopath(U.get(l), tempvector)) {
                                    xx++;
                                    break;
                                }

                            }
                            //If candidate node is not on chosen set of Useed (U) added candidate node to chosen set of Useed to add as new CIset.
                            if (xx == 0) {
                                U.add(tempvector);
                            }
                            if (CIset.size() == maxNumberofCandidate) {
                                exit = true;
                                break;
                            }
                        } else {
                            tempvector.remove(tempvector.size() - 1);
                        }
                    }
                    if (exit) {
                        break;
                    }
                }
                if (exit) {
                    break;
                }
                z = 1;
            }

            Useed.clear();
            Useed = U;

            List<List<Integer>> Utemp = new ArrayList<>();
            List<Integer> tempvector1;
            tempvalue1 = 0;

            //if ((k == maxCIsize - 1|| exit == true || Useed.get(Useed.size() - 1).size() < NprimeOrder.size() + 1) && exceptionone()) {
            if ((k == maxCIsize - 1) && exceptionone()){
                for (int i = 0; i < elementaryNodes.size(); i++) {

                    // for each node that made path of size zero, we add this node to all of CIset.
                    if (elementryPathArray[0][i] == -2) {

                        for (int j = 0; j < Useed.size(); j++) {
                            tempvector1 = new ArrayList<>();
                            tempvector1 = Useed.get(j);
                            tempvector1.add(elementaryNodes.get(i));
                            int x = 0;
                            for (int l = 0; l < CIset.size(); l++) {
                                if (comapringtwopath(CIset.get(l), tempvector1)) {

                                    x = 1;
                                    break;
                                }
                            }
                            if (x == 0) {

                                int xx = 0;

                                for (int l = 0; l < Utemp.size(); l++) {
                                    if (comapringtwopath(Utemp.get(l), tempvector1)) {
                                        xx++;
                                        break;
                                    }

                                }
                                if (xx == 0) {
                                    Utemp.add(tempvector1);
                                }
                            }
                        }
                    }

                }
                Useed.clear();
                Useed = Utemp;
            }
            for (int i = 0; i < Useed.size(); i++) {

                for (int j = 0; j < elementarypathstemp.length; j++) {
                    elementarypathstemp[j] = 0;
                }

                for (int l = 0; l < elementarypaths.size(); l++) {
                    for (int j = 0; j < Useed.get(i).size(); j++) {

                        if (elementarypaths.get(l).contains(Useed.get(i).get(j))) {
                            elementarypathstemp[l] = 1;
                            break;
                        }

                    }
                }
                tempvalue1 = 0;
                for (int j = 0; j < elementarypathstemp.length; j++) {
                    if (elementarypathstemp[j] == 1) {
                        tempvalue1++;
                    }
                }

                //If it covered all of paths we check to see whether CIset has subset of these Useed element or not if not we add it to CIset other we keep this Useed for next iteration that
                //means candidate for CIset with size bigger.
                if (tempvalue1 == elementarypathstemp.length) {

                    int x = 0;
                    for (int l = 0; l < CIset.size(); l++) {
                        if (comapringtwopath(CIset.get(l), Useed.get(i))) {

                            x = 1;
                            break;
                        }
                    }
                    if (x == 0)
                        CIset.add(Useed.get(i));
                }
            }
        }
    }

    /**
     * check whether path1 is subset of path2 or not
     * @return true iff path1 is subset of path2 and false,  if it is not
     *
     * @param two paths Path1 and Path2
     */
    private boolean comapringtwopath(List<Integer> oldpath, List<Integer> newpath) {
        boolean equal = true;

        int temp = 0;

        //Check all of nodes on path 1 by path2 to find number of nodes that are similar on both path
        for (int i = 0; i < oldpath.size(); i++) {

            if (newpath.contains(oldpath.get(i))) {

                temp++;
            }
        }

        //If all of elements of path1 happened on Path2 then equal is true otherwise it is false.
        if (temp != oldpath.size()) {
            equal = false;
        }
        return equal;
    }

    /**
     * check whether current has at least one element of  path1 or not
     * @return true iff current has at least one element of  path1 and false,  if it is not
     *
     * @param two paths Path1 and current
     */
    public boolean cover(List<Integer> current, List<Integer> Path1) {

        boolean cover = false;

        //For all element of set current we check whether it happened on set Path1 or not, if there is output is true otherwise output is false.
        for (int i = 0; i < current.size(); i++) {

            if (Path1.contains(current.get(i))) {

                cover = true;
                break;
            }
        }
        return cover;
    }

    /**
     * @return ocsana score of given path[summation of ocsana score of each node on a this path]
     *
     * @param Path, one elementary path
     */
    public float setPathScore(List<Integer> Path) {

        float CIscore = 0;

        int temp = 0;

        for (int i = 0; i < Path.size(); i++) {

            //Each cell on path vector contains hash mapping of one node. this line, finds index of this node on elementary vector, ocsanaScore vector has same order of elementary nodes vector.
            temp = elementaryNodes.indexOf(Path.get(i));

            //Add ocsana score of current node to CIscore of path
            CIscore = CIscore + ocsanaScore.get(temp);
        }
        return CIscore;
    }

    /**
     * Sort CIset based on ocsana score of each set
     *
     * @param
     */
    private void CIsort(){
        CIocsanascore = new float[CIset.size()];

        for (int i = 0; i < CIset.size(); i++) {
            CIocsanascore[i] = setPathScore(CIset.get(i));
        }
        for (int i = 0; i < CIocsanascore.length - 1; i++) {

            for (int j = i + 1; j < CIocsanascore.length; j++) {

                if (CIocsanascore[i] > CIocsanascore[j]) {

                    float t = CIocsanascore[i];
                    CIocsanascore[i] = CIocsanascore[j];
                    CIocsanascore[j] = t;

                    List<Integer> tt = CIset.get(i);

                    CIset.set(i, CIset.get(j));
                    CIset.set(j, tt);
                }
            }
        }

    }

    /**
     * @return CIset as solution of hitting set problem
     *
     * @param
     */
    public List<List<Integer>>  getCIset(){

        return CIset;
    }

    /**
     * @return CIocsanascore as ocsana score of each set on CISET
     *
     * @param
     */
    public float[] getCIocsanascore(){

        return CIocsanascore;

    }

}
