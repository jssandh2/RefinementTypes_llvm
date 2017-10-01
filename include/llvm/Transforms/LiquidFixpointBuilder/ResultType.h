#ifndef LLVM_TRANSFORMS_UTILS_LIQUID_RESULTTYPE_H
#define LLVM_TRANSFORMS_UTILS_LIQUID_RESULTTYPE_H

#include <string>
#include "llvm/Transforms/LiquidFixpointBuilder/RefinementMacroUtils.h"

class NODISCARD ResultType
{
private:
	ResultType(bool succeeded, std::string errorMsg) : ErrorMsg(errorMsg),Succeeded(succeeded) {}

public:

	ResultType(const ResultType& obj) : ErrorMsg(obj.ErrorMsg), Succeeded(obj.Succeeded) {}

	const std::string ErrorMsg;
	const bool Succeeded;

	static ResultType Success()
	{
		ResultType ret(true, "");
		return ret;
	}

	static ResultType Error(std::string errorMsg)
	{
		ResultType ret(false, errorMsg);
		return ret;
	}
};
#endif
