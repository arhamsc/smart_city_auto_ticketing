import express, { NextFunction, Request, Response } from "express";
import logger from "morgan";
import ExpressError from "./middlewares/expressError.js";
import debugM from "debug";
import "./configs/prisma_client.js";
import userRoutes from "./routes/user.routes.js";
import busRoutes from "./routes/bus.routes.js";
import routeRoutes from "./routes/route.routes.js";
import ticketingRoutes from "./routes/ticketing.routes.js";

const app = express();
const debug = debugM("bmtc_pro_api");

app.use(logger("dev"));
app.use(express.json());
app.use(express.urlencoded({ extended: false }));

// app.get("/", (req: Request, res: Response) => {
//   res.send("Working");
// });

app.use("/users", userRoutes);
app.use("/bus", busRoutes);
app.use("/route", routeRoutes);
app.use("/ticket", ticketingRoutes);

// error handler
app.use(function (
  err: ExpressError,
  req: Request,
  res: Response,
  next: NextFunction,
) {
  // set locals, only providing error in development
  res.locals.message = err.message;
  res.locals.error = req.app.get("env") === "development" ? err : {};

  // render the error page
  res.status(err.status || 500);
  res.render("error");
});

app.listen(3000, () => {
  console.log("Listening on port 3000");
});
