#ifndef __EXCEPTION__HPP__
#define __EXCEPTION__HPP__

/*******************************************************************
 * origin author: Yao Yue (Tripack45)                              *
 * *************************************************************** *
 * FIRST OCCUR IN APPROXIMATE LOGIC SYNTHESIS                      *
 * *************************************************************** *
 * modifier: Su Hang (AnthonySu), Zhou Zhuangzhuang (NathanZhou)   *
 *******************************************************************/

class Exception {};

class ExceptionDataset :    public Exception {};
class ExceptionGenerator :  public Exception {};

class ExceptionDatasetUninitialized :   public ExceptionDataset {};
class ExceptionOutOfRange :             public ExceptionDataset {};

class ExceptionGeneratorEmpty : public ExceptionGenerator {};

class ExceptionTruthTable : public Exception {};

class ExceptionInvalidVectorAssignment : public ExceptionTruthTable {};
class ExceptionTruthTableInvalidSize : public ExceptionTruthTable {};

static void throwFalse(bool i) {if(!i) throw i;}


#endif