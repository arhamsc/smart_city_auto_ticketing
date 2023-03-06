import { Route } from "@prisma/client";
import { NextFunction, Request, Response } from "express";
import ExpressError from "../middlewares/expressError.js";
import {
  addRoute,
  getAllRoutes,
  getRoutesById,
} from "../services/route.service.js";

export const createRoute = async (
  req: Request,
  res: Response,
  next: NextFunction,
) => {
  try {
    const { body } = req;
    let route = body satisfies Route;
    const newRoute = await addRoute(route);
    return res.json({ ...newRoute });
  } catch (error: any) {
    next(new ExpressError({}));
  }
};

export const getRoutes = async (
  req: Request,
  res: Response,
  next: NextFunction,
) => {
  try {
    const { query } = req;
    if (query.id !== undefined) {
      const route = await getRoutesById(query?.id as string);
      return res.json({ route });
    }
    const routes = await getAllRoutes();
    return res.json(routes);
  } catch (error: any) {
    next(new ExpressError({}));
    console.log(error);
  }
};
