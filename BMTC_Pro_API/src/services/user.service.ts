import { User } from "@prisma/client";
import SmartCityDB from "../configs/prisma_client.js";

const prisma = SmartCityDB.getPrismaClient();

export const addUser = async (user: User) => {
  try {
    const newUser = await prisma.user.create({
      data: user,
    });

    return {
      user: newUser,
      message: "User created.",
    };
  } catch (error: any) {
    return {
      message: error.message,
    };
  }
};

export const getAllUsers = async () => {
  try {
    const allUsers = await prisma.user.findMany({});
    return {
      users: allUsers,
    };
  } catch (error: any) {
    return {
      message: error.message,
    };
  }
};

export const getUsersById = async (id: string) => {
  try {
    const user = await prisma.user.findUnique({
      where: {
        id,
      },
    });
    return {
      user,
    };
  } catch (error: any) {
    return {
      message: error.message,
    };
  }
};
