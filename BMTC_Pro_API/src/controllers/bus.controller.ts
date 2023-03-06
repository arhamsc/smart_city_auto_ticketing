import { Bus } from "@prisma/client";
import { NextFunction, Request, Response } from "express";
import ExpressError from "../middlewares/expressError.js";
import { addBus, getAllBuses, getBusById } from "../services/bus.service.js";

export const createBus = async (
  req: Request,
  res: Response,
  next: NextFunction,
) => {
  try {
    const { body } = req;
    let bus = body satisfies Bus;

    const newBus = await addBus(bus);
    return res.json({ ...newBus });
  } catch (error: any) {
    next(new ExpressError({}));
  }
};

export const getBuses = async (
  req: Request,
  res: Response,
  next: NextFunction,
) => {
  try {
    const { query } = req;
    if (query.id !== undefined) {
      const bus = await getBusById(query?.id as string);
      return res.json({ bus });
    }
    const buses = await getAllBuses();
    return res.json(buses);
  } catch (error: any) {
    next(new ExpressError({}));
    console.log(error);
  }
};
