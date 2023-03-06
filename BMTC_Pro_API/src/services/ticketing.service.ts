import SmartCityDB from "../configs/prisma_client.js";

const prisma = SmartCityDB.getPrismaClient();

export const assignBoarding = async (
  email: string,
  busNo: string,
  boardingStation: string,
) => {
  try {
    const user = await prisma.user.findUnique({
      where: {
        email: email.replace("\r", ""),
      },
    });
    const bus = await prisma.bus.findUnique({
      where: {
        regNum: busNo,
      },
    });

    const present = await prisma.boarding.findFirst({
      where: {
        AND: [
          {
            userId: user?.id,
            busId: bus?.id,
          },
        ],
      },
    });
    if (present) {
      return {
        message: "Already exists",
      };
    }

    const boarding = await prisma.boarding.create({
      data: {
        boardingLocation: boardingStation,
        Bus: {
          connect: {
            id: bus?.id,
          },
        },
        user: {
          connect: {
            id: user?.id,
          },
        },
      },
    });
    console.log(boarding);
    return {
      boarding,
    };
  } catch (error: any) {
    return {
      message: error.message,
    };
  }
};
