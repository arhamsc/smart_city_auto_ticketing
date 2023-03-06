import { Route } from "@prisma/client";
import SmartCityDB from "../configs/prisma_client.js";

const prisma = SmartCityDB.getPrismaClient();

export const addRoute = async (route: Route) => {
  try {
    const newRoute = await prisma.route.create({
      data: route,
    });

    return {
      route: newRoute,
      message: "Route created.",
    };
  } catch (error: any) {
    return {
      message: error.message,
    };
  }
};

export const getAllRoutes = async () => {
  try {
    const allRoutes = await prisma.route.findMany({});
    return {
      routes: allRoutes,
    };
  } catch (error: any) {
    return {
      message: error.message,
    };
  }
};

export const getRoutesById = async (id: string) => {
  try {
    const route = await prisma.route.findUnique({
      where: {
        id,
      },
    });
    return {
      route,
    };
  } catch (error: any) {
    return {
      message: error.message,
    };
  }
};
