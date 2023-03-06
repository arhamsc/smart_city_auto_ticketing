import { User } from "@prisma/client";
import { NextFunction, Request, Response } from "express";
import ExpressError from "../middlewares/expressError.js";
import {
  addUser,
  getAllUsers,
  getUsersById,
} from "../services/user.service.js";

export const createUser = async (
  req: Request,
  res: Response,
  next: NextFunction,
) => {
  try {
    const { body } = req;
    let user = body satisfies User;

    const newUser = await addUser(user);
    return res.json({ ...newUser });
  } catch (error: any) {
    next(new ExpressError({}));
  }
};

export const getUsers = async (
  req: Request,
  res: Response,
  next: NextFunction,
) => {
  try {
    const { query } = req;
    if (query.id !== undefined) {
      const user = await getUsersById(query?.id as string);
      return res.json({ user });
    }
    const users = await getAllUsers();
    return res.json(users);
  } catch (error: any) {
    next(new ExpressError({}));
    console.log(error);
  }
};
