import { Bus } from "@prisma/client";
import SmartCityDB from "../configs/prisma_client.js";

const prisma = SmartCityDB.getPrismaClient();

export const addBus = async (bus: Bus) => {
  try {
    const newBus = await prisma.bus.create({
      data: bus,
    });

    return {
      bus: newBus,
      message: "Bus created.",
    };
  } catch (error: any) {
    return {
      message: error.message,
    };
  }
};

export const getAllBuses = async () => {
  try {
    const allBuses = await prisma.bus.findMany({});
    return {
      buses: allBuses,
    };
  } catch (error: any) {
    return {
      message: error.message,
    };
  }
};

export const getBusById = async (id: string) => {
  try {
    const bus = await prisma.bus.findUnique({
      where: {
        id,
      },
    });
    return {
      bus,
    };
  } catch (error: any) {
    return {
      message: error.message,
    };
  }
};
