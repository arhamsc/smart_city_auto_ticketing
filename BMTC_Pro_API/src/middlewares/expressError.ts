import { CustomError } from "../typings/customError.js";

class ExpressError extends Error {
  status: number;

  constructor(errorBody: CustomError) {
    super();
    this.message = errorBody.message ?? "Oops! Something went wrong.";
    this.status = errorBody.status ?? 500;
  }
}

export default ExpressError;
